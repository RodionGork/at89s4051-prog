unsigned char linebuf[128];
unsigned char page[32];

void setup() {
  pinMode(10, INPUT); // mosi
  pinMode(11, INPUT); // miso
  pinMode(12, INPUT); // sck
  pinMode(9, OUTPUT); // rst
  digitalWrite(9, LOW);
  Serial.begin(1200);
  delay(100);
}

int hex(unsigned char c) {
  if (c <= '9') {
    return c - '0';
  }
  return c - 'A' + 10;
}

int xchbyte(int v) {
  delayMicroseconds(10);
  int mask = 0x80;
  int res = 0;
  while (mask) {
    digitalWrite(12, HIGH);
    digitalWrite(10, (v & mask) ? HIGH : LOW);
    delayMicroseconds(10);
    digitalWrite(12, LOW);
    res <<= 1;
    if (digitalRead(11) == HIGH) {
      res |= 1;
    }
    mask >>= 1;
    delayMicroseconds(10);
  }
  return res;
}

int xch4bytes(int cmd, int p1, int p2, int v) {
  xchbyte(cmd);
  xchbyte(p1);
  xchbyte(p2);
  return xchbyte(v);
}

void prgEnable() {
  xch4bytes(0b10101100, 0b01010011, 0, 0);
}

int readSignature() {
  return xch4bytes(0b00101000, 0, 0, 0);
}

void eraseChip() {
  xch4bytes(0b10101100, 0b10000000, 0, 0);
}

void writeByte(int addr, int b) {
  xch4bytes(0b01000000, addr >> 8, addr & 0xFF, b);
  delay(3);
}

void dbg(char* s) {
  Serial.println(s);
}

void dbg1(char* s, int v) {
  Serial.print(s);
  Serial.print(' ');
  Serial.println(v);
}

int readChar() {
  while (1) {
    int c = Serial.read();
    if (c >= 0) {
      return c;
    }
  }
}

void readLine() {
  int p = 0;
  while (1) {
    int c = readChar();
    if (c > ' ') {
      linebuf[p++] = c;
      break;
    }
  }
  while (1) {
    int c = readChar();
    if (c == -1) {
      continue;
    }
    if (c <= ' ') {
      linebuf[p] = 0;
      break;
    }
    linebuf[p] = c;
    if (p < sizeof(linebuf) - 1) {
      p += 1;
    }
  }
}

int byteAt(int i) {
  i = i * 2 + 1;
  return (hex(linebuf[i]) << 4) | hex(linebuf[i + 1]);
}

void pageClean() {
  for (int i = 0; i < sizeof(page); i++) {
    page[i] = 0xFF;
  }
}

void writePage(int pg) {
  Serial.print("Page ");
  Serial.print(pg);
  Serial.print(" ");
  xchbyte(0b01010000);
  xchbyte(pg >> 3);
  xchbyte((pg & 0x1F) << 5);
  for (int i = 0; i < sizeof(page); i++) {
    xchbyte(page[i]);
  }
  delay(5);
  xchbyte(0b00110000);
  xchbyte(pg >> 3);
  xchbyte((pg & 0x1F) << 5);
  int ok = 0;
  for (int i = 0; i < sizeof(page); i++) {
    int v = xchbyte(0);
    if (v == page[i]) {
      ok += 1;
    }
  }
  Serial.println(ok);
}

int process(void) {
  int pg, prevPg = -1;
  int offs;
  pageClean();
  while (1) {
    readLine();
    int sz = byteAt(0);
    int addr = (byteAt(1) << 8) + byteAt(2);
    int flag = byteAt(3);
    dbg1("Size", sz);
    dbg1("Addr", addr);
    dbg1("Flag", flag);
    if (flag != 0) {
      break;
    }
    pg = addr / sizeof(page);
    if (prevPg != -1 && prevPg != pg) {
      writePage(prevPg);
      pageClean();
      prevPg = -1;
    }
    offs = addr % sizeof(page);
    for (int i = 0; i < sz; i++) {
      prevPg = pg;
      page[offs] = byteAt(i + 4);
      offs += 1;
      if (offs >= sizeof(page)) {
        writePage(prevPg);
        pageClean();
        prevPg = -1;
        pg += 1;
        offs = 0;
      }
    }
  }
  if (prevPg != -1) {
    writePage(prevPg);
    pageClean();
  }
}

void loop() {
  delay(100);
  if (Serial.peek() < 0) {
    return;
  }
  pinMode(12, OUTPUT);
  digitalWrite(12, LOW);
  delay(1);
  digitalWrite(9, HIGH);
  delay(4);
  pinMode(10, OUTPUT);
  Serial.println("PRG EN");
  prgEnable();
  int sig = readSignature();
  Serial.print("SIG=");
  Serial.println(sig);
  Serial.println("ERASE");
  eraseChip();
  delay(10);
  process();
  delay(10);
  digitalWrite(9, LOW);
  pinMode(12, INPUT);
  pinMode(10, INPUT);
  delay(100);
  digitalWrite(9, HIGH);
  delay(100);
  digitalWrite(9, LOW);
  Serial.println("DONE");
  while (Serial.peek() >= 0) {
    Serial.read();
  }
}
