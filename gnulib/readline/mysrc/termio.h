#pragma once
#include <stdint.h>

// c_lflag
#define ISIG    0000001   /* Enable signals.  */
#define ICANON  0000002   /* Canonical input (erase and kill processing).  */
#define ECHO    0000010   /* Enable echo.  */
#define ECHOE	0000020   /* Echo erase character as error-correcting backspace.*/
#define ECHOK	0000040   /* Echo KILL.  */
#define ECHONL	0000100   /* Echo NL.  */
#define NOFLSH	0000200   /* Disable flush after interrupt or quit.  */
#define TOSTOP	0000400   /* Send SIGTTOU for background output.  */
#define ECHOCTL 0001000  /* If ECHO is also set, terminal special characters
				 other than TAB, NL, START, and STOP are echoed as
				 ^X, where X is the character with ASCII code 0x40
				 greater than the special character
				 (not in POSIX).  */
#define ECHOPRT 0002000  /* If ICANON and ECHO are also set, characters are
				 printed as they are being erased
				 (not in POSIX).  */
#define ECHOKE	 0004000  /* If ICANON is also set, KILL is echoed by erasing
				 each character on the line, as specified by ECHOE
				 and ECHOPRT (not in POSIX).  */
#define FLUSHO	 0010000  /* Output is being flushed.  This flag is toggled by
				 typing the DISCARD character (not in POSIX).  */
#define PENDIN	 0040000  /* All characters in the input queue are reprinted
				 when the next character is read (not in POSIX).  */
#define IEXTEN	0100000   /* Enable implementation-defined input processing.  */



/* c_cflag bit meaning */
#define  B0	0000000		/* hang up */
#define  B50	0000001
#define  B75	0000002
#define  B110	0000003
#define  B134	0000004
#define  B150	0000005
#define  B200	0000006
#define  B300	0000007
#define  B600	0000010
#define  B1200	0000011
#define  B1800	0000012
#define  B2400	0000013
#define  B4800	0000014
#define  B9600	0000015
#define  B19200	0000016
#define  B38400	0000017

#define CSIZE   0000060
#define   CS5   0000000
#define   CS6   0000020
#define   CS7   0000040
#define   CS8   0000060
#define CSTOPB  0000100
#define CREAD   0000200
#define PARENB  0000400
#define PARODD  0001000
#define HUPCL   0002000
#define CLOCAL  0004000

// iflag
#define IGNBRK  0000001  /* Ignore break condition.  */
#define BRKINT  0000002  /* Signal interrupt on break.  */
#define IGNPAR  0000004  /* Ignore characters with parity errors.  */
#define PARMRK  0000010  /* Mark parity and framing errors.  */
#define INPCK   0000020  /* Enable input parity check.  */
#define ISTRIP  0000040  /* Strip 8th bit off characters.  */
#define INLCR   0000100  /* Map NL to CR on input.  */
#define IGNCR   0000200  /* Ignore CR.  */
#define ICRNL   0000400  /* Map CR to NL on input.  */
#define IUCLC   0001000  /* Map uppercase characters to lowercase on input
                (not in POSIX).  */
#define IXON    0002000  /* Enable start/stop output control.  */
#define IXANY   0004000  /* Enable any character to restart output.  */
#define IXOFF   0010000  /* Enable start/stop input control.  */
#define IMAXBEL 0020000  /* Ring bell when input queue is full
                (not in POSIX).  */
#define IUTF8   0040000  /* Input is UTF8 (not in POSIX).  */

// oflag
#define OPOST	0000001  /* Post-process output.  */
#define OLCUC	0000002  /* Map lowercase characters to uppercase on output.
                (not in POSIX).  */
#define ONLCR	0000004  /* Map NL to CR-NL on output.  */
#define OCRNL	0000010  /* Map CR to NL on output.  */
#define ONOCR	0000020  /* No CR output at column 0.  */
#define ONLRET	0000040  /* NL performs CR function.  */
#define OFILL	0000100  /* Use fill characters for delay.  */
#define OFDEL	0000200  /* Fill is DEL.  */

// c_cc
#define VINTR 0
#define VQUIT 1
#define VERASE 2
#define VKILL 3
#define VEOF 4
#define VEOL 5
#define VTIME 5  //vtime may be veol
#define VMIN 6

#define NCCS 8
struct termio
{
    uint32_t c_iflag;       /* input mode flags */
    uint32_t c_oflag;       /* output mode flags */
    uint32_t c_cflag;       /* control mode flags */
    uint32_t c_lflag;       /* local mode flags */
    uint8_t c_line;            /* line discipline */
    uint8_t c_cc[NCCS];        /* control characters */
};

#define TCGETA      0x5405
#define TCSETA      0x5406
#define TCSETAW     0x5407
#define TIOCGWINSZ    0x5413
#define TIOCSWINSZ    0x5414


struct winsize {
    unsigned short ws_row;
    unsigned short ws_col;
};
