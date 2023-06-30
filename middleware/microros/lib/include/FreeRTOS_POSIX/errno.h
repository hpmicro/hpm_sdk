/*
 * Amazon FreeRTOS POSIX V1.1.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
 * @file errno.h
 * @brief System error numbers.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/errno.h.html
 *
 * The values defined in this file may not be compatible with the strerror
 * function provided by this system.
 */

#ifndef _FREERTOS_POSIX_ERRNO_H_
#define _FREERTOS_POSIX_ERRNO_H_

/* Undefine all errnos to avoid redefinition errors with system errnos. */
#undef EPERM
#undef ENOENT
#undef EBADF
#undef EAGAIN
#undef ENOMEM
#undef EEXIST
#undef EBUSY
#undef EINVAL
#undef ENOSPC
#undef ERANGE
#undef ENAMETOOLONG
#undef EDEADLK
#undef EOVERFLOW
#undef ENOSYS
#undef EMSGSIZE
#undef ENOTSUP
#undef ETIMEDOUT

/**
 * @name Definition of POSIX errnos.
 */
/**@{ */
#define EPERM               1
#define ENOENT              2
#define ESRCH               3
#define EINTR               4
#define EIO                 5
#define ENXIO               6
#define E2BIG               7
#define E2BIG_STR           "Arg list too long"
#define ENOEXEC             8
#define EBADF               9
#define ECHILD              10
#define EAGAIN              11
#define EWOULDBLOCK         EAGAIN
#define ENOMEM              12
#define EACCES              13
#define EFAULT              14                         /* Linux errno extension */
#define ENOTBLK             15
#define EBUSY               16
#define EEXIST              17
#define EXDEV               18
#define ENODEV              19
#define ENOTDIR             20
#define EISDIR              21
#define EINVAL              22
#define ENFILE              23
#define EMFILE              24
#define ENOTTY              25
#define ETXTBSY             26
#define EFBIG               27
#define ENOSPC              28
#define ESPIPE              29
#define EROFS               30
#define EMLINK              31
#define EPIPE               32
#define EDOM                33
#define ERANGE              34
#define ENOMSG              35
#define EIDRM               36
#define ECHRNG              37                         /* Linux errno extension */
#define EL2NSYNC            38                         /* Linux errno extension */
#define EL2NSYNC_STR        "Level 2 not synchronized"
#define EL3HLT              39                         /* Linux errno extension */
#define EL3HLT_STR          "Level 3 halted"
#define EL3RST              40                         /* Linux errno extension */
#define EL3RST_STR          "Level 3 reset"
#define ELNRNG              41                         /* Linux errno extension */
#define EUNATCH             42                         /* Linux errno extension */
#define ENOCSI              43                         /* Linux errno extension */
#define EL2HLT              44                         /* Linux errno extension */
#define EL2HLT_STR          "Level 2 halted"
#define EDEADLK             45
#define ENOLCK              46

#define EBADE               50                         /* Linux errno extension */
#define EBADR               51                         /* Linux errno extension */
#define EXFULL              52                         /* Linux errno extension */
#define ENOANO              53                         /* Linux errno extension */
#define EBADRQC             54                         /* Linux errno extension */
#define EBADSLT             55                         /* Linux errno extension */
#define EDEADLOCK           56                         /* Linux errno extension */
#define EBFONT              57                         /* Linux errno extension */

#define ENOSTR              60
#define ENODATA             61
#define ETIME               62
#define ENOSR               63
#define ENONET              64                         /* Linux errno extension */
#define ENOPKG              65                         /* Linux errno extension */
#define EREMOTE             66                         /* Linux errno extension */
#define ENOLINK             67
#define EADV                68                         /* Linux errno extension */
#define ESRMNT              69                         /* Linux errno extension */
#define ECOMM               70                         /* Linux errno extension */
#define EPROTO              71

#define EMULTIHOP           74
#define ELBIN               75                         /* Linux errno extension */
#define EDOTDOT             76                         /* Linux errno extension */
#define EBADMSG             77

#define EFTYPE              79
#define ENOTUNIQ            80                         /* Linux errno extension */
#define EBADFD              81                         /* Linux errno extension */
#define EREMCHG             82                         /* Linux errno extension */
#define ELIBACC             83                         /* Linux errno extension */
#define ELIBBAD             84                         /* Linux errno extension */
#define ELIBSCN             85                         /* Linux errno extension */
#define ELIBMAX             86                         /* Linux errno extension */
#define ELIBEXEC            87                         /* Linux errno extension */
#define ENOSYS              88
#define ENMFILE             89                         /* Cygwin */
#define ENOTEMPTY           90
#define ENAMETOOLONG        91
#define ELOOP               92

#define EOPNOTSUPP          95
#define EPFNOSUPPORT        96

#define ECONNRESET          104
#define ENOBUFS             105
#define EAFNOSUPPORT        106
#define EPROTOTYPE          107
#define ENOTSOCK            108
#define ENOPROTOOPT         109
#define ESHUTDOWN           110                         /* Linux errno extension */
#define ECONNREFUSED        111
#define EADDRINUSE          112
#define ECONNABORTED        113
#define ENETUNREACH         114
#define ENETDOWN            115
#define ETIMEDOUT           116
#define EHOSTDOWN           117
#define EHOSTUNREACH        118
#define EINPROGRESS         119
#define EALREADY            120
#define EDESTADDRREQ        121
#define EMSGSIZE            122
#define EPROTONOSUPPORT     123
#define ESOCKTNOSUPPORT     124                         /* Linux errno extension */
#define EADDRNOTAVAIL       125
#define ENETRESET           126
#define EISCONN             127
#define ENOTCONN            128
#define ETOOMANYREFS        129
#define EPROCLIM            130
#define EUSERS              131
#define EDQUOT              132
#define ESTALE              133
#define ENOTSUP             134
#define ENOMEDIUM           135                         /* Linux errno extension */
#define ENOSHARE            136                         /* Cygwin */
#define ECASECLASH          137                         /* Cygwin */
#define EILSEQ              138
#define EOVERFLOW           139
#define ECANCELED           140
#define ENOTRECOVERABLE     141
#define EOWNERDEAD          142
#define ESTRPIPE            143                         /* Linux errno extension */

/**@} */

/**
 * @name System Variable
 *
 * @brief Define FreeRTOS+POSIX errno, if enabled.
 * Set configUSE_POSIX_ERRNO to enable, and clear to disable. See FreeRTOS.h.
 *
 * @{
 */


// TODO (Pablogs): Check where is the place to include this configuration

extern int FreeRTOS_errno;
#define errno    FreeRTOS_errno
/**@} */

#endif /* ifndef _FREERTOS_POSIX_ERRNO_H_ */
