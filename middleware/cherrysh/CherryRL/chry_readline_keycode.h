/*
 * Copyright (c) 2022, Egahp
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*!< VT101 User Guide                                                           */
/*!< APPENDIX C                                                                 */
/*!< <1>    Standard Key Character Codes (0x21-0x7E)                            */
/*!< Lowercase      Lowercase Code      Uppercase       Uppercasee Code         */
/*!< a              0x61                A               0x41                    */
/*!< b              0x62                B               0x42                    */
/*!< c              0x63                C               0x43                    */
/*!< d              0x64                D               0x44                    */
/*!< e              0x65                E               0x45                    */
/*!< f              0x66                F               0x46                    */
/*!< g              0x67                G               0x47                    */
/*!< h              0x68                H               0x48                    */
/*!< i              0x69                I               0x49                    */
/*!< j              0x6A                J               0x4A                    */
/*!< k              0x6B                K               0x4B                    */
/*!< l              0x6C                L               0x4C                    */
/*!< m              0x6D                M               0x4D                    */
/*!< n              0x6E                N               0x4E                    */
/*!< o              0x6F                O               0x4F                    */
/*!< p              0x70                P               0x50                    */
/*!< q              0x71                Q               0x51                    */
/*!< r              0x72                R               0x52                    */
/*!< s              0x73                S               0x53                    */
/*!< t              0x74                T               0x54                    */
/*!< u              0x75                U               0x55                    */
/*!< v              0x76                V               0x56                    */
/*!< w              0x77                W               0x57                    */
/*!< x              0x78                X               0x58                    */
/*!< y              0x79                Y               0x59                    */
/*!< z              0x7A                Z               0x5A                    */
/*!< 1              0x31                !               0x21                    */
/*!< 2              0x32                @               0x40                    */
/*!< 3              0x33                #               0x23                    */
/*!< 4              0x34                $               0x24                    */
/*!< 5              0x35                %               0x25                    */
/*!< 6              0x36                ^(circumflex)   0x5E                    */
/*!< 7              0x37                &               0x26                    */
/*!< 8              0x38                *               0x2A                    */
/*!< 9              0x39                (               0x28                    */
/*!< 0              0x30                )               0x29                    */
/*!< -(minus)       0x2D                _               0x5F                    */
/*!< =              0x3D                +               0x2B                    */
/*!< `(grave)       0x60                ~(tilde)        0x7E                    */
/*!< [              0x5B                {               0x7B                    */
/*!< ]              0x5D                }               0x7D                    */
/*!< ;              0x3B                :               0x3A                    */
/*!< '(spostrophe)  0x27                "               0x22                    */
/*!< \              0x5C                |               0x7C                    */
/*!< ,(comma)       0x2C                <               0x3C                    */
/*!< .(period)      0x2E                >               0x3E                    */
/*!< /              0x2F                ?               0x3F                    */
/*!<                                                                            */
/*!< <2>    Cursor Control KeyCodes                                             */
/*!<    Cursor Key      ANSI Mode Reset ANSI Mode Set   VT52 Mode               */
/*!<    ↑   Up          <esc>[A         <esc>OA         <esc>A                  */
/*!<    ↓   Down        <esc>[B         <esc>OB         <esc>B                  */
/*!<    →   Right       <esc>[C         <esc>OC         <esc>C                  */
/*!<    ←   Left        <esc>[D         <esc>OD         <esc>D                  */
/*!<                                                                            */
/*!< <3>    Control Codes Generated (0x00 - 0x1F, 0x7F)                         */
/*!<    Mnemonic        Code Hex        Key with CTRL   Dedicated Key           */
/*!<    NUL             0x00            Spcae Bar                               */
/*!<    SOH             0x01            A                                       */
/*!<    STX             0x02            B                                       */
/*!<    ETX             0x03            C                                       */
/*!<    EOT             0x04            D                                       */
/*!<    ENQ             0x05            E                                       */
/*!<    ACK             0x06            F                                       */
/*!<    BEL             0x07            G                                       */
/*!<    BS              0x08            H               BACK SPACE              */
/*!<    HT              0x09            I               TAB                     */
/*!<    LF              0x0A            J               LINE FEED               */
/*!<    VT              0x0B            K                                       */
/*!<    FF              0x0C            L                                       */
/*!<    CR              0x0D            M               RETURN (ENTER)          */
/*!<    SO              0x0E            N                                       */
/*!<    SI              0x0F            O                                       */
/*!<    DLE             0x10            P                                       */
/*!<    DC1             0x11            Q                                       */
/*!<    DC2             0x12            R                                       */
/*!<    DC3             0x13            S                                       */
/*!<    DC4             0x14            T                                       */
/*!<    NAK             0x15            U                                       */
/*!<    SYN             0x16            V                                       */
/*!<    ETB             0x17            W                                       */
/*!<    CAN             0x18            X                                       */
/*!<    EM              0x19            Y                                       */
/*!<    SUB             0x1A            Z                                       */
/*!<    ESC             0x1B            [                                       */
/*!<    FS              0x1C            \                                       */
/*!<    GS              0x1D            ]                                       */
/*!<    RS              0x1E            ~                                       */
/*!<    US              0x1F            ?                                       */
/*!<    DEL             0x7F                            DELETE                  */
/*!<                                                                            */
/*!< The following control charaters are generated differently from previous    */
/*!<    NUL             0x00            @                                       */
/*!<    RS              0x1E            ^                                       */
/*!<    US              0x1F            - &                                     */
/*!<                                                                            */
/*!< <4>    Auxiliary Keypad Codes                                              */
/*!<    Key     ANSI Numeric    ANSI Alternate  VT52 Numeric    VT52 Alternate  */
/*!<    0       0               <esc>Op         0               <esc>?p         */
/*!<    1       1               <esc>Oq         1               <esc>?q         */
/*!<    2       2               <esc>Or         2               <esc>?r         */
/*!<    3       3               <esc>Os         3               <esc>?s         */
/*!<    4       4               <esc>Ot         4               <esc>?t         */
/*!<    5       5               <esc>Ou         5               <esc>?u         */
/*!<    6       6               <esc>Ov         6               <esc>?v         */
/*!<    7       7               <esc>Ow         7               <esc>?w         */
/*!<    8       8               <esc>Ox         8               <esc>?x         */
/*!<    9       9               <esc>Oy         9               <esc>?y         */
/*!<    -       -               <esc>Om         -               <esc>?m         */
/*!<    ,       ,               <esc>Ol         ,               <esc>?l         */
/*!<    .       .               <esc>On         .               <esc>?n         */
/*!<    ENTER   CR              <esc>OM         CR              <esc>?M         */
/*!<    PF1     <esc>OP         <esc>OP         <esc>P          <esc>?P         */
/*!<    PF2     <esc>OQ         <esc>OQ         <esc>Q          <esc>?Q         */
/*!<    PF3     <esc>OR         <esc>OR         <esc>R          <esc>?R         */
/*!<    PF4     <esc>OS         <esc>OS         <esc>S          <esc>?S         */
/*!<                                                                            */
/*!<                                                                            */
/*!< vt sequences:                                                              */
/*!< <esc>[1~    - Home        <esc>[16~   -             <esc>[31~   - F17      */
/*!< <esc>[2~    - Insert      <esc>[17~   - F6          <esc>[32~   - F18      */
/*!< <esc>[3~    - Delete      <esc>[18~   - F7          <esc>[33~   - F19      */
/*!< <esc>[4~    - End         <esc>[19~   - F8          <esc>[34~   - F20      */
/*!< <esc>[5~    - PgUp        <esc>[20~   - F9          <esc>[35~   -          */
/*!< <esc>[6~    - PgDn        <esc>[21~   - F10                                */
/*!< <esc>[7~    - Home        <esc>[22~   -                                    */
/*!< <esc>[8~    - End         <esc>[23~   - F11                                */
/*!< <esc>[9~    -             <esc>[24~   - F12                                */
/*!< <esc>[10~   - F0          <esc>[25~   - F13                                */
/*!< <esc>[11~   - F1          <esc>[26~   - F14                                */
/*!< <esc>[12~   - F2          <esc>[27~   -                                    */
/*!< <esc>[13~   - F3          <esc>[28~   - F15                                */
/*!< <esc>[14~   - F4          <esc>[29~   - F16                                */
/*!< <esc>[15~   - F5          <esc>[30~   -                                    */
/*!<                                                                            */
/*!< xterm sequences:                                                           */
/*!< <esc>[A     - Up          <esc>[K     -             <esc>[U     -          */
/*!< <esc>[B     - Down        <esc>[L     -             <esc>[V     -          */
/*!< <esc>[C     - Right       <esc>[M     -             <esc>[W     -          */
/*!< <esc>[D     - Left        <esc>[N     -             <esc>[X     -          */
/*!< <esc>[E     -             <esc>[O     -             <esc>[Y     -          */
/*!< <esc>[F     - End         <esc>[P     - F1          <esc>[Z     -          */
/*!< <esc>[G     - Keypad 5    <esc>[Q     - F2                                 */
/*!< <esc>[H     - Home        <esc>[R     - F3                                 */
/*!< <esc>[I     -             <esc>[S     - F4                                 */
/*!< <esc>[J     -             <esc>[T     -                                    */
/*!<                                                                            */
/*!< Host -> Terminal input sequences                                           */
/*!< Pressing special keys on the keyboard, as well as outputting many          */
/*!< xterm CSI, DCS, or OSC sequences, often produces a CSI, DCS, or OSC        */
/*!< sequence,  sent from the terminal to the computer as though the user       */
/*!< typed it.                                                                  */
/*!< When typing input on a terminal keypresses outside the normal main         */
/*!< alphanumeric keyboard area can be sent to the host as ANSI sequences.      */
/*!< For keys that have an equivalent output function, such as the cursor       */
/*!< keys, these often mirror the output sequences. However, for most           */
/*!< keypresses there isn't an equivalent output sequence to use.               */
/*!<                                                                            */
/*!< There are several encoding schemes, and unfortunately most terminals       */
/*!< mix sequences from different schemes, so host software has to be able      */
/*!< to deal  with input sequences using any scheme. To complicate the          */
/*!< matter, the VT  terminals themselves have two schemes of input, normal     */
/*!< mode and application mode that can be switched by the application.         */
/*!<                                                                            */
/*!<                                                                            */
/*!< Pn     ->  Numeric Parameter                                               */
/*!<    A parameter that represents a number, designated by Pn.                 */
/*!< Ps     ->  Selective Parameter                                             */
/*!<    A parameter that selects a subfunction from a specified list of         */
/*!<    subfunctions, designated by Ps. In general, a control sequence with     */
/*!<    more than one selective parameter causes the same effect as several     */
/*!<    control sequences, each with one selective parameter, e.g.,             */
/*!<    CSI Psa; Psb; Psc F is identical to CSI Psa F CSI Psb F CSI Psc F.      */
/*!< Parameter String                                                           */
/*!<    A string of parameters separated by a semicolon -> ;                    */
/*!< Default                                                                    */
/*!<    A function-dependent value that is assumed when no explicit value,      */
/*!<    or a value of 0, is specified.                                          */
/*!< F      ->  Final character                                                 */
/*!<    A character whose bit combination terminates an escape or control       */
/*!<    sequence.                                                               */
/*!<                                                                            */
/*!< <esc> c        ->      RIS Reset to initial State                          */
/*!< <esc> N        ->      SS2 Single Shift Select of G2 Character Set         */
/*!< <esc> O        ->      SS3 Single Shift Select of G3 Character Set         */
/*!< <esc> P        ->      DCS Device Control String                           */
/*!< <esc> [        ->      CSI Control Sequence Introducer                         */
/*!< <esc> \        ->      ST  String Terminator                                       */
/*!< <esc> ]        ->      OSC Operating System Command                                    */
/*!<                                                                                            */
/*!< CSI Ps A       ->      CUU Cursor Up                       Cursor Up Ps Times                  */
/*!< CSI Ps B       ->      CUD Cursor Down                     Cursor Down Ps Times                */
/*!< CSI Ps C       ->      CUF Cursor Forward                  Cursor Forward Ps Times             */
/*!< CSI Ps D       ->      CUB Cursor Back                     Cursor Backward Ps Times            */
/*!< CSI Ps E       ->      CNL Cursor Next Line                Cursor Next Line Ps Times           */
/*!< CSI Ps F       ->      CPL Cursor Previous Line            Cursor Previous Line Ps Times       */
/*!< CSI Ps G       ->      CHA Cursor Horizontal Absolute      Cursor Character Absolute [column]  */
/*!< CSI Ps ; Ps H  ->      CUP Cursor Position                 Cursor Position [row;column]        */
/*!< CSI Ps J       ->      ED Erase in Display                                                     */
/*!<                            Ps = 0 -> Erase Below (default)                                     */
/*!<                            Ps = 1 -> Erase Above                                               */
/*!<                            Ps = 2 -> Erase All                                                 */
/*!<                            Ps = 3 -> Erase Saved Lines (xterm)                                 */
/*!< CSI Ps K       ->      EL Erase in Line                                                        */
/*!<                            Ps = 0 -> Erase to Right (default)                                  */
/*!<                            Ps = 1 -> Erase to Left                                             */
/*!<                            Ps = 2 -> Erase All                                                 */
/*!< CSI Ps L       ->      IL  Insert Line                     Insert Ps Lines                     */
/*!< CSI Ps M       ->      DL  Delete Line                     Delete Ps Lines                     */
/*!< CSI Ps S       ->      SU  Scroll Up                       Scroll up Ps lines                  */
/*!< CSI Ps T       ->      SD  Scroll Down                     Scroll down Ps lines                */
/*!< CSI Ps n       ->      DSR Device Status Report                                                */
/*!<                            Ps = 5 -> Status Report CSI 0 n                                     */
/*!<                            Ps = 6 -> Report Cursor Position [row;column] as CSI r ; c R        */
/*!< CSI s          ->      SCP Save Current Cursor Position                                        */
/*!< CSI u          ->      RCP Restore Saved Cursor Position                                       */
/*!< CSI Ps t       ->      WM  Window manipulation                                                 */
/*!<                            Ps = 18 -> Report the size of the text area in characters           */
/*!<                                as CSI 8 ; height ; width t                                     */
/*!< CSI ? Ps h     ->      DECSET DEC Private Mode Set                                             */
/*!<                            Ps = 47 -> UUse Alternate Screen Buffer                             */
/*!< CSI ? Ps l     ->      DECRST DEC Private Mode Reset                                           */
/*!<                            Ps = 47 -> Use Normal Screen Buffer                                 */

#ifndef CHRY_READLINE_KEYCODE_H
#define CHRY_READLINE_KEYCODE_H

#ifdef __cplusplus
extern "C" {
#endif

enum {
    /*!< C0 control codes for general use                                       */
    CHRY_READLINE_C0_NUL = 0x00, /*!< Null                      \0              */
    CHRY_READLINE_C0_SOH = 0x01, /*!< Start of Heading                          */
    CHRY_READLINE_C0_STX = 0x02, /*!< Start of Text                             */
    CHRY_READLINE_C0_ETX = 0x03, /*!< End of Text                               */
    CHRY_READLINE_C0_EOT = 0x04, /*!< End of Transmission                       */
    CHRY_READLINE_C0_ENQ = 0x05, /*!< Enquiry                                   */
    CHRY_READLINE_C0_ACK = 0x06, /*!< Acknowledge                               */
    CHRY_READLINE_C0_BEL = 0x07, /*!< Bell, Alert               \a              */
    CHRY_READLINE_C0_BS = 0x08,  /*!< Backspace                 \b              */
    CHRY_READLINE_C0_HT = 0x09,  /*!< Horizontal Tabulation     \t              */
    CHRY_READLINE_C0_LF = 0x0A,  /*!< Line Feed                 \n              */
    CHRY_READLINE_C0_VT = 0x0B,  /*!< Vertical Tabulation       \v              */
    CHRY_READLINE_C0_FF = 0x0C,  /*!< Form Feed                 \f              */
    CHRY_READLINE_C0_CR = 0x0D,  /*!< Carriage Return           \r              */
    CHRY_READLINE_C0_SO = 0x0E,  /*!< Shift Out                                 */
    CHRY_READLINE_C0_SI = 0x0F,  /*!< Shift In                                  */
    CHRY_READLINE_C0_DLE = 0x10, /*!< Data Link Escape                          */
    CHRY_READLINE_C0_DC1 = 0x11, /*!< Device Control One        XON             */
    CHRY_READLINE_C0_DC2 = 0x12, /*!< Device Control Two                        */
    CHRY_READLINE_C0_DC3 = 0x13, /*!< Device Control Three      XOFF            */
    CHRY_READLINE_C0_DC4 = 0x14, /*!< Device Control Four                       */
    CHRY_READLINE_C0_NAK = 0x15, /*!< Negative Acknowledge                      */
    CHRY_READLINE_C0_SYN = 0x16, /*!< Synchronous Idle                          */
    CHRY_READLINE_C0_ETB = 0x17, /*!< End of Transmission Block                 */
    CHRY_READLINE_C0_CAN = 0x18, /*!< Cancel                                    */
    CHRY_READLINE_C0_EM = 0x19,  /*!< End of medium                             */
    CHRY_READLINE_C0_SUB = 0x1A, /*!< Substitute                                */
    CHRY_READLINE_C0_ESC = 0x1B, /*!< Escape                    \e              */
    CHRY_READLINE_C0_FS = 0x1C,  /*!< File Separator                            */
    CHRY_READLINE_C0_GS = 0x1D,  /*!< Group Separator                           */
    CHRY_READLINE_C0_RS = 0x1E,  /*!< Record Separator                          */
    CHRY_READLINE_C0_US = 0x1F,  /*!< Unit Separator                            */
    /*!< While not technically part of the C0 control  character range, the     */
    /*!< following two characters are defined in ISO/IEC 2022 as always being   */
    /*!< available regardless of which sets of control characters and graphics  */
    /*!< characters have been registered. They can be thought of as having some */
    /*!< characteristics of control characters.                                 */
    CHRY_READLINE_C0_SP = 0x20,  /*!< Space                                     */
    CHRY_READLINE_G0_BEG = 0x20, /*!< G0 Code Start                             */
    CHRY_READLINE_G0_END = 0x7E, /*!< G0 Code End                               */
    CHRY_READLINE_C0_DEL = 0x7F, /*!< Delete                                    */
    /*!< Not support 8bit codes                                                 */
};

enum {
    CHRY_READLINE_EXEC_NUL = 0,   /*!< Null                      */
    CHRY_READLINE_EXEC_CLR,       /*!< Clear screen refresh line */
    CHRY_READLINE_EXEC_NLN,       /*!< Newline                   */
    CHRY_READLINE_EXEC_ALN,       /*!< Abortline                 */
    CHRY_READLINE_EXEC_DEL,       /*!< Delete character          */
    CHRY_READLINE_EXEC_BS,        /*!< Backspace character       */
    CHRY_READLINE_EXEC_MVRT,      /*!< Move Right                */
    CHRY_READLINE_EXEC_MVLT,      /*!< Move Left                 */
    CHRY_READLINE_EXEC_MVED,      /*!< Move End                  */
    CHRY_READLINE_EXEC_MVHM,      /*!< Move Home                 */
    CHRY_READLINE_EXEC_NXTH,      /*!< Next History              */
    CHRY_READLINE_EXEC_PRVH,      /*!< Prev History              */
    CHRY_READLINE_EXEC_DLWD,      /*!< Delete prev word          */
    CHRY_READLINE_EXEC_DHLN,      /*!< Delete whole line         */
    CHRY_READLINE_EXEC_DELN,      /*!< Delete cursor to end line */
    CHRY_READLINE_EXEC_CPLT,      /*!< Complete                  */
    CHRY_READLINE_EXEC_ACPLT,     /*!< Autocomplete or space     */
    CHRY_READLINE_EXEC_SWNM,      /*!< Switch to Normal Screen   */
    CHRY_READLINE_EXEC_HELP,      /*!< Show help                 */
    CHRY_READLINE_EXEC_EOF_,      /*!< End of file               */
    CHRY_READLINE_EXEC_SIGINT_,   /*!< SIGINT                    */
    CHRY_READLINE_EXEC_EOF,       /*!< End of file               */
    CHRY_READLINE_EXEC_SIGINT,    /*!< SIGINT                    */
    CHRY_READLINE_EXEC_SIGQUIT,   /*!< SIGQUIT                   */
    CHRY_READLINE_EXEC_SIGCONT,   /*!< SIGTCONT                  */
    CHRY_READLINE_EXEC_SIGSTOP,   /*!< SIGSTOP                   */
    CHRY_READLINE_EXEC_SIGTSTP,   /*!< SIGTSTP                   */
    CHRY_READLINE_EXEC_F1,        /*!< Fucntion 1                */
    CHRY_READLINE_EXEC_F2,        /*!< Fucntion 2                */
    CHRY_READLINE_EXEC_F3,        /*!< Fucntion 3                */
    CHRY_READLINE_EXEC_F4,        /*!< Fucntion 4                */
    CHRY_READLINE_EXEC_F5,        /*!< Fucntion 5                */
    CHRY_READLINE_EXEC_F6,        /*!< Fucntion 6                */
    CHRY_READLINE_EXEC_F7,        /*!< Fucntion 7                */
    CHRY_READLINE_EXEC_F8,        /*!< Fucntion 8                */
    CHRY_READLINE_EXEC_F9,        /*!< Fucntion 9                */
    CHRY_READLINE_EXEC_F10,       /*!< Fucntion 10               */
    CHRY_READLINE_EXEC_F11,       /*!< Fucntion 11               */
    CHRY_READLINE_EXEC_F12,       /*!< Fucntion 12               */
    CHRY_READLINE_EXEC_USER,      /*!< User Event Start          */
    CHRY_READLINE_EXEC_END = 255, /*!< Max Value is 255          */
};

enum {
    CHRY_READLINE_CTRLMAP_A = 1, /*!< defalut is MVHM   */
    CHRY_READLINE_CTRLMAP_B,     /*!< default is MVLT   */
    CHRY_READLINE_CTRLMAP_C,     /*!< default is ALN    */
    CHRY_READLINE_CTRLMAP_D,     /*!< default is DEL    */
    CHRY_READLINE_CTRLMAP_E,     /*!< default is MVED   */
    CHRY_READLINE_CTRLMAP_F,     /*!< default is MVRT   */
    CHRY_READLINE_CTRLMAP_G,     /*!< default is ALN    */
    CHRY_READLINE_CTRLMAP_H,     /*!< default is BS     */
    CHRY_READLINE_CTRLMAP_I,     /*!< default is CPLT   */
    CHRY_READLINE_CTRLMAP_J,     /*!< default is NLN    */
    CHRY_READLINE_CTRLMAP_K,     /*!< default is DELN   */
    CHRY_READLINE_CTRLMAP_L,     /*!< default is CLR    */
    CHRY_READLINE_CTRLMAP_M,     /*!< default is NLN    */
    CHRY_READLINE_CTRLMAP_N,     /*!< default is NXTH   */
    CHRY_READLINE_CTRLMAP_O,     /*!< default is NLN    */
    CHRY_READLINE_CTRLMAP_P,     /*!< default is PRVH   */
    CHRY_READLINE_CTRLMAP_Q,     /*!< default is NUL    */
    CHRY_READLINE_CTRLMAP_R,     /*!< default is NUL    */
    CHRY_READLINE_CTRLMAP_S,     /*!< default is NUL    */
    CHRY_READLINE_CTRLMAP_T,     /*!< default is NUL    */
    CHRY_READLINE_CTRLMAP_U,     /*!< default is DHLN   */
    CHRY_READLINE_CTRLMAP_V,     /*!< default is NUL    */
    CHRY_READLINE_CTRLMAP_W,     /*!< default is DLWD   */
    CHRY_READLINE_CTRLMAP_X,     /*!< default is NUL    */
    CHRY_READLINE_CTRLMAP_Y,     /*!< default is NUL    */
    CHRY_READLINE_CTRLMAP_Z,     /*!< default is ALN    */
};

enum {
    CHRY_READLINE_ALTMAP_A = 0, /*!< default is NUL     */
    CHRY_READLINE_ALTMAP_B,     /*!< default is NUL     */
    CHRY_READLINE_ALTMAP_C,     /*!< default is NUL     */
    CHRY_READLINE_ALTMAP_D,     /*!< default is NUL     */
    CHRY_READLINE_ALTMAP_E,     /*!< default is NUL     */
    CHRY_READLINE_ALTMAP_F,     /*!< default is NUL     */
    CHRY_READLINE_ALTMAP_G,     /*!< default is NUL     */
    CHRY_READLINE_ALTMAP_H,     /*!< default is NUL     */
    CHRY_READLINE_ALTMAP_I,     /*!< default is NUL     */
    CHRY_READLINE_ALTMAP_J,     /*!< default is NUL     */
    CHRY_READLINE_ALTMAP_K,     /*!< default is NUL     */
    CHRY_READLINE_ALTMAP_L,     /*!< default is NUL     */
    CHRY_READLINE_ALTMAP_M,     /*!< default is NUL     */
    CHRY_READLINE_ALTMAP_N,     /*!< default is NUL     */
    CHRY_READLINE_ALTMAP_O,     /*!< default is NUL     */
    CHRY_READLINE_ALTMAP_P,     /*!< default is NUL     */
    CHRY_READLINE_ALTMAP_Q,     /*!< default is NUL     */
    CHRY_READLINE_ALTMAP_R,     /*!< default is NUL     */
    CHRY_READLINE_ALTMAP_S,     /*!< default is NUL     */
    CHRY_READLINE_ALTMAP_T,     /*!< default is NUL     */
    CHRY_READLINE_ALTMAP_U,     /*!< default is NUL     */
    CHRY_READLINE_ALTMAP_V,     /*!< default is NUL     */
    CHRY_READLINE_ALTMAP_W,     /*!< default is NUL     */
    CHRY_READLINE_ALTMAP_X,     /*!< default is NUL     */
    CHRY_READLINE_ALTMAP_Y,     /*!< default is NUL     */
    CHRY_READLINE_ALTMAP_Z,     /*!< default is NUL     */
};

#ifdef __cplusplus
}
#endif

#endif
