|Pos|R/W|Name                   |Default|Note                                              |
|---|---|-----------------------|-------|--------------------------------------------------|
|0  |R  |                       |0x88   |                                                  |
|1  |R  |                       |0x00   |                                                  |
|2  |RW |                       |0x00   |                                                  |
|3  |RW |                       |0xf1   |                                                  |
|4  |RW |                       |0x18   |                                                  |
|5  |RW |                       |0x90   |                                                  |
|6  |RW |                       |0x20   |                                                  |
|7  |RW |                       |0x0b   |                                                  |
|8  |RW |                       |0x20   |                                                  |
|9  |RW |                       |0x09   |                                                  |
|A  |RW |                       |0x20   |                                                  |
|B  |RW |                       |0x09   |                                                  |
|C  |RW |                       |0x08   |                                                  |
|D  |RW |                       |0x20   |                                                  |
|E  |RW |                       |0x10   |                                                  |
|F  |R  |                       |0x00   |                                                  |
|10 |RW |                       |0x80   |                                                  |
|11 |RW |filter                 |0x38   |[3] - disable sharpening                          |
|12 |RW |                       |0x00   |                                                  |
|13 |RW |                       |0x00   |                                                  |
|14 |RW |                       |0x00   |                                                  |
|15 |RW |x/y shift              |0x88   |[0:3] - y shift [7:4] - x shift                   |
|16 |RW |x fine shift           |0x95   |[7:0] shift amount                                |
|17 |RW |y fine shift           |0x18   |[7:0] shift amount                                |
|18 |RW |                       |0x00   |                                                  |
|19 |RW |Color format?          |0x00   |                                                  |
|1A |RW |                       |0x25   |[3] - show x cross on screen                      |
|1B |R  |                       |0x32   |                                                  |
|1C |RW |                       |0x78   |                                                  |
|1D |RW |                       |0x00   |                                                  |
|1E |R  |                       |0x40   |                                                  |
|1F |RW |                       |0x64   |                                                  |
|20 |RW |                       |0x00   |                                                  |
|21 |RW |                       |0x58   |                                                  |
|22 |RW |                       |0x4e   |                                                  |
|23 |RW |                       |0x2c   |                                                  |
|24 |RW |                       |0x30   |                                                  |
|25 |RW |                       |0x18   |                                                  |
|26 |RW |                       |0x20   |                                                  |
|27 |RW |                       |0x03   |                                                  |
|28 |RW |                       |0x08   |                                                  |
|29 |RW |                       |0x60   |                                                  |
|2A |RW |                       |0x86   |                                                  |
|2B |RW |                       |0x1c   |                                                  |
|2C |RW |                       |0x00   |                                                  |
|2D |RW |                       |0x07   |                                                  |
|2E |RW |                       |0x23   |                                                  |
|2F |RW |                       |0x5f   |                                                  |
|30 |R  |                       |0x1b   |                                                  |
|31 |RW |Brightness             |0x7c   |                                                  |
|32 |RW |Contrast               |0x8e   |                                                  |
|33 |RW |Saturation             |0x8a   |                                                  |
|34 |RW |Hue                    |0xa0   |[5:0] - Hue                                       |
|35 |RW |                       |0x98   |[0:0] - Swap red and blue channel [5:3] - red horizotal shift|
|36 |RW |                       |0x00   |                                                  |
|37 |RW |                       |0x50   |                                                  |
|38 |RW |another brightness?    |0x3c   |                                                  |
|39 |RW |another contrast?      |0x61   |                                                  |
|3A |R  |                       |0x01   |                                                  |
|3B |RW |Red adj                |0x80   |                                                  |
|3C |RW |Green adj              |0x80   |                                                  |
|3D |RW |Blue adj               |0x80   |                                                  |
|3E |RW |fine red adj?          |0x80   |                                                  |
|3F |RW |fine green adj?        |0x80   |                                                  |
|40 |RW |fine blue adj?         |0x80   |                                                  |
|41 |RW |some color tone adj    |0x60   |[5:0] color ajd. [7:6] nothing?                   |
|42 |RW |                       |0x5f   |                                                  |
|43 |RW |                       |0x37   |                                                  |
|44 |RW |                       |0x14   |                                                  |
|45 |RW |test patern            |0x00   |[0:2] -test patern, [3] - show test patern        |
|46 |RW |                       |0xc8   |                                                  |
|47 |RW |                       |0xff   |                                                  |
|48 |RW |                       |0x80   |                                                  |
|49 |RW |more color adj         |0x00   |[0:6] - adj, [7] - enable                         |
|4A |R  |                       |0x00   |                                                  |
|4B |RW |                       |0x20   |                                                  |
|4C |R  |                       |0x00   |                                                  |
|4D |R  |                       |0x00   |                                                  |
|4E |R  |                       |0x00   |                                                  |
|4F |R  |                       |0x7f   |                                                  |
|50 |RW |                       |0x20   |                                                  |
|51 |R  |                       |0x20   |                                                  |
|52 |RW |                       |0x00   |                                                  |
|53 |R  |                       |0x04   |                                                  |
|54 |R  |                       |0x00   |                                                  |
|55 |RW |                       |0xa0   |                                                  |
|56 |RW |                       |0x40   |                                                  |
|57 |RW |                       |0x04   |                                                  |
|58 |R  |                       |0x10   |                                                  |
|59 |R  |                       |0x08   |                                                  |
|5A |R  |                       |0x10   |                                                  |
|5B |RW |                       |0x00   |                                                  |
|5C |RW |                       |0x68   |                                                  |
|5D |RW |                       |0x84   |                                                  |
|5E |R  |                       |0x08   |                                                  |
|5F |R  |                       |0x21   |                                                  |
|60 |RW |                       |0x20   |                                                  |
|61 |RW |                       |0xb0   |                                                  |
|62 |RW |                       |0x33   |                                                  |
|63 |RW |                       |0x01   |                                                  |
|64 |RW |                       |0xff   |                                                  |
|65 |RW |                       |0x01   |                                                  |
|66 |RW |                       |0xff   |                                                  |
|67 |RW |                       |0x89   |                                                  |
|68 |RW |                       |0xab   |                                                  |
|69 |RW |                       |0x28   |                                                  |
|6A |RW |                       |0x6e   |                                                  |
|6B |RW |                       |0x6e   |                                                  |
|6C |RW |                       |0x6e   |                                                  |
|6D |RW |                       |0x00   |                                                  |
|6E |RW |                       |0x00   |                                                  |
|6F |RW |                       |0x00   |                                                  |
|70 |R  |                       |0x00   |                                                  |
|71 |RW |                       |0x00   |                                                  |
|72 |RW |                       |0x00   |                                                  |
|73 |RW |                       |0x00   |                                                  |
|74 |RW |                       |0x00   |                                                  |
|75 |RW |                       |0xff   |                                                  |
|76 |RW |                       |0xff   |                                                  |
|77 |RW |                       |0xff   |                                                  |
|78 |RW |                       |0xff   |                                                  |
|79 |RW |                       |0xff   |                                                  |
|7A |RW |                       |0xff   |                                                  |
|7B |RW |                       |0xff   |                                                  |
|7C |RW |                       |0xff   |                                                  |
|7D |RW |                       |0xf0   |                                                  |
|7E |RW |                       |0x80   |                                                  |
|7F |RW |                       |0x00   |                                                  |
|80 |RW |                       |0x00   |                                                  |
|81 |RW |                       |0x20   |make screen go green / black                      |
|82 |RW |                       |0x70   |[5,7] - change color space  [6] - make screen dark|
|83 |RW |another y shift (up)   |0x25   |                                                  |
|84 |RW |another x shift (right)|0xfa   |                                                  |
|85 |RW |another y shift (down) |0x0f   |                                                  |
|86 |RW |                       |0x05   |                                                  |
|87 |RW |                       |0x14   |                                                  |
|88 |RW |                       |0x00   |                                                  |
|89 |RW |                       |0x00   |                                                  |
|8A |RW |                       |0x80   |                                                  |
|8B |RW |                       |0x7e   |                                                  |
|8C |RW |                       |0x03   |[3] - half screen interleave                      |
|8D |RW |another x shift (left) |0x48   |                                                  |
|8E |RW |coarse shift x/y       |0x70   |                                                  |
|8F |RW |                       |0x00   |                                                  |
|90 |RW |                       |0x70   |                                                  |
|91 |R  |                       |0x01   |                                                  |
|92 |RW |                       |0xc2   |                                                  |
|93 |R  |                       |0x74   |                                                  |
|94 |RW |                       |0xd0   |                                                  |
|95 |RW |                       |0x80   |                                                  |
|96 |R  |                       |0x65   |                                                  |
|97 |RW |                       |0x77   |                                                  |
|98 |RW |                       |0x02   |[6] flip x, [7] flip y                            |
|99 |RW |                       |0xd0   |                                                  |
|9A |R  |                       |0x20   |                                                  |
|9B |RW |                       |0x08   |                                                  |
|9C |R  |                       |0x00   |                                                  |
|9D |RW |stress x               |0x80   |                                                  |
|9E |RW |                       |0x04   |                                                  |
|9F |RW |                       |0x65   |                                                  |
|A0 |RW |                       |0x50   |                                                  |
|A1 |RW |X pos                  |0x80   |                                                  |
|A2 |RW |                       |0xd0   |                                                  |
|A3 |RW |                       |0x77   |                                                  |
|A4 |RW |                       |0x00   |                                                  |
|A5 |RW |                       |0x80   |                                                  |
|A6 |RW |                       |0x00   |                                                  |
|A7 |RW |                       |0x70   |                                                  |
|A8 |RW |                       |0x40   |                                                  |
|A9 |RW |                       |0x1e   |                                                  |
|AA |RW |                       |0x0c   |                                                  |
|AB |RW |                       |0x00   |                                                  |
|AC |R  |                       |0x90   |                                                  |
|AD |R  |                       |0x65   |                                                  |
|AE |R  |                       |0x65   |                                                  |
|AF |RW |                       |0x15   |                                                  |
|B0 |R  |                       |0x00   |                                                  |
|B1 |R  |                       |0x00   |                                                  |
|B2 |RW |                       |0x00   |                                                  |
|B3 |RW |                       |0x00   |                                                  |
|B4 |R  |                       |0x00   |                                                  |
|B5 |RW |                       |0x00   |                                                  |
|B6 |RW |                       |0x00   |                                                  |
|B7 |R  |                       |0x00   |                                                  |
|B8 |RW |line draw ctl 1        |0x00   |color border around top and left side of screen   |
|B9 |RW |line draw ctl 2        |0x00   |move line around                                  |
|BA |RW |line draw ctl 3        |0x00   |move line fine X                                  |
|BB |RW |line draw ctl 4        |0x00   |move line fine Y                                  |
|BC |R  |                       |0x00   |                                                  |
|BD |RW |                       |0x20   |                                                  |
|BE |R  |                       |0x00   |                                                  |
|BF |R  |                       |0x00   |                                                  |
|C0 |R  |                       |0x00   |                                                  |
|C1 |R  |                       |0x00   |                                                  |
|C2 |R  |                       |0x00   |                                                  |
|C3 |R  |                       |0x00   |                                                  |
|C4 |R  |                       |0x00   |                                                  |
|C5 |R  |                       |0x00   |                                                  |
|C6 |R  |                       |0x00   |                                                  |
|C7 |R  |                       |0x00   |                                                  |
|C8 |R  |                       |0x00   |                                                  |
|C9 |R  |                       |0x00   |                                                  |
|CA |R  |                       |0x00   |                                                  |
|CB |R  |                       |0x00   |                                                  |
|CC |R  |                       |0x00   |                                                  |
|CD |R  |                       |0x00   |                                                  |
|CE |R  |                       |0x00   |                                                  |
|CF |R  |                       |0x00   |                                                  |
|D0 |R  |                       |0xfc   |                                                  |
|D1 |R  |                       |0xff   |                                                  |
|D2 |R  |                       |0x08   |                                                  |
|D3 |R  |                       |0xfc   |                                                  |
|D4 |R  |                       |0x01   |                                                  |
|D5 |R  |                       |0x10   |                                                  |
|D6 |RW |                       |0x00   |                                                  |
|D7 |RW |                       |0x10   |                                                  |
|D8 |RW |                       |0x00   |                                                  |
|D9 |R  |                       |0x00   |                                                  |
|DA |RW |                       |0x77   |                                                  |
|DB |RW |                       |0x00   |                                                  |
|DC |RW |                       |0x00   |                                                  |
|DD |RW |                       |0x00   |                                                  |
|DE |R  |                       |0x88   |                                                  |
|DF |R  |                       |0x98   |                                                  |
|E0 |R  |                       |0x6c   |                                                  |
|E1 |R  |                       |0x44   |                                                  |
|E2 |R  |                       |0x65   |                                                  |
|E3 |R  |                       |0x51   |                                                  |
|E4 |R  |                       |0x00   |                                                  |
|E5 |R  |                       |0xaf   |                                                  |
|E6 |R  |                       |0xf6   |                                                  |
|E7 |RW |                       |0xf0   |                                                  |
|E8 |R  |                       |0x00   |                                                  |
|E9 |RW |                       |0x00   |                                                  |
|EA |RW |                       |0x00   |                                                  |
|EB |RW |                       |0x00   |                                                  |
|EC |RW |                       |0x00   |                                                  |
|ED |R  |                       |0x48   |                                                  |
|EE |R  |                       |0x6c   |                                                  |
|EF |R  |                       |0x51   |                                                  |
|F0 |RW |                       |0x00   |[6:0] - test patern [7] - enable test             |
|F1 |RW |                       |0x43   |                                                  |
|F2 |RW |                       |0x1f   |                                                  |
|F3 |RW |                       |0x1f   |                                                  |
|F4 |RW |                       |0x22   |                                                  |
|F5 |RW |                       |0x73   |                                                  |
|F6 |RW |                       |0x53   |                                                  |
|F7 |RW |                       |0x80   |                                                  |
|F8 |R  |                       |0x01   |                                                  |
|F9 |RW |                       |0x01   |                                                  |
|FA |R  |                       |0x00   |                                                  |
|FB |RW |                       |0x00   |                                                  |
|FC |RW |                       |0x00   |                                                  |
|FD |R  |                       |0x00   |                                                  |
|FE |RW |                       |0x00   |                                                  |
|FF |RW |                       |0x00   |                                                  |
