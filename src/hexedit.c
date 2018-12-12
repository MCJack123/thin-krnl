#include <hexedit.h>

extern unsigned char current_settings;
extern const char* hexnums;

#define hnib(n) (n & 0xF0) >> 4
#define lnib(n) n & 0x0F

void htoa_(unsigned int n, int z, char* buffer) {
    char* bytes = (char*)&n;
    int i = 0;
    buffer[0] = '0';
    buffer[1] = 'x';
    buffer[z*2+2] = '\0';

    for (i = 0; i < z; i++) {
        buffer[z*2+1-(i*2)] = hexnums[lnib(bytes[i])];
        buffer[z*2-(i*2)] = hexnums[hnib(bytes[i])];
    }
}

void write_header(unsigned int offset) {
    char b[10];
    movec(0, 0);
    htoa_(offset, 4, b);
    print(b);
    print("---------------------------------------------------------------------");
}

void write_footer() {
    move(24 * 160);
    print("-------------------------------------------------------------------------------");
}

void write_line(void* offset, int line, int selected) {
    int i;
    char strtmp[2];
    char htemp[10];
    offset = (unsigned int)offset & ~0x0F;
    movec(0, line);
    htoa_(offset, 4, htemp);
    print(htemp);
    print(" | ");
    for (i = 0; i < 16; i++) {
        if (selected == i) current_settings = 0x87;
        else current_settings = 0x07;
        htoa_(((char*)offset)[i], 1, htemp);
        print(htemp + 2);
        current_settings = 0x07;
        print(" ");
        if (selected == i) current_settings = 0x87;
        else current_settings = 0x07;
        movec(63+i, line);
        strtmp[0] = ((char*)offset)[i];
        if (strtmp[0] < 32 || strtmp[0] > 128) strtmp[0] = '.';
        print(strtmp);
        movec((3*i)+16, line);
    }
    movec(61, line);
    current_settings = 0x07;
    print("|");
}

#define co(l, c) (l<<4)+c

extern char convert_ps2_code(unsigned char);

void memedit(char* offset) {
    int code, line = 0, column = 0, partial = -1, i;
    bool runLoop = true, runLines = true, cont = false;
    char ch;
    offset = (unsigned int)offset & ~0x0F;
    clear();
    while (runLoop) {
        write_header((unsigned int)offset + co(line, column));
        write_footer();
        for (i = 0; i < 23; i++) write_line(offset + co(i, column), i + 1, line == i ? column : -1);
        while (runLines) {
            movec((column * 3) + 13 + (partial != -1), line + 1);
            code = getkey();
            cont = false;
            switch (code) {
                case 0xE048: // up
                    if (partial == -1) {
                        if (line <= 0) {
                            if (offset < 16) break;
                            offset = offset - 16;
                            runLines = false;
                            line = 0;
                        } else line--;
                        write_line(offset + co(line+1, column), line + 2, -1);
                        write_line(offset + co(line, column), line + 1, column);
                    }
                    break;
                case 0xE050: // down
                    if (partial == -1) {
                        if (line >= 22) {
                            offset = offset + 16;
                            runLines = false;
                            line = 22;
                        } else line++;
                        write_line(offset + co(line-1, column), line, -1);
                        write_line(offset + co(line, column), line + 1, column);
                    }
                    break;
                case 0xE04B: // left
                    if (partial == -1 && column != 0) {
                        column--;
                        write_line(offset + co(line, column), line + 1, column);
                    }
                    break;
                case 0xE04D: // right
                    if (partial == -1 && column != 15) {
                        column++;
                        write_line(offset + co(line, column), line + 1, column);
                    }
                    break;
                default:
                    cont = true;
            }
            if (!cont || code > 255) {
                write_header((unsigned int)offset + co(line, column));
                continue;
            }
            ch = convert_ps2_code((unsigned char)code);
            if (ch == 'q') {
                runLoop = false;
                break;
            } else if (ch >= '0' && ch <= '9') {
                ch -= '0';
                if (partial == -1) {
                    offset[co(line, column)] = (ch << 4) | (offset[co(line, column)] & 0x0F);
                    partial = ch;
                } else {
                    offset[co(line, column)] = ch | (partial << 4);
                    partial = -1;
                    if (column == 15) {
                        column = 0;
                        if (line == 22) {
                            offset = offset + 16;
                            runLines = false;
                        } else line++;
                        write_line(offset + co(line-1, column), line, -1);
                    } else column++;
                }
                write_line(offset + co(line, column), line + 1, column);
            } else if (ch >= 'a' && ch <= 'f') {
                ch -= 'a' - 10;
                if (partial == -1) {
                    offset[co(line, column)] = (ch << 4) | (offset[co(line, column)] & 0x0F);
                    partial = ch;
                } else {
                    offset[co(line, column)] = ch | (partial << 4);
                    partial = -1;
                    if (column == 15) {
                        column = 0;
                        if (line == 22) {
                            offset = offset + 16;
                            runLines = false;
                        } else line++;
                        write_line(offset + co(line-1, column), line, -1);
                    } else column++;
                }
                write_line(offset + co(line, column), line + 1, column);
            } else if (ch >= 'A' && ch <= 'F') { // probably redundant
                ch -= 'A' - 10;
                if (partial == -1) {
                    offset[co(line, column)] = (ch << 4) | (offset[co(line, column)] & 0x0F);
                    partial = ch;
                } else {
                    offset[co(line, column)] = ch | (partial << 4);
                    partial = -1;
                    if (column == 15) {
                        column = 0;
                        if (line == 22) {
                            offset = offset + 16;
                            runLines = false;
                        } else line++;
                        write_line(offset + co(line-1, column), line, -1);
                    } else column++;
                }
                write_line(offset + co(line, column), line + 1, column);
            }
        }
        runLines = true;
    }
    clear();
}