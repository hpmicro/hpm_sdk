# Change Log

## [1.0.0] - 2024-03-09:

All changes

### Added
  - first commit
  - update readme
  - add change log
  - [feat] add noblock mode, CONFIG_READLINE_NOBLOCK
  - [feat] add prompt auto refresh, CONFIG_READLINE_REFRESH_PROMPT
  - [fix] disable completion in mask mode
  - [feat] callback add param chry_readline_t *rl
  - [fix] fix chry_readline_calculate_prompt pptsize check
  - [fix] fix some function error return value
  - [fix] fix unused variable warning
  - [fix] fix param check in history disabled mode
  - [feat] merge function callback and user event callback
  - [feat] add EXEC EOF_;SIGINT_;EOF;SIGINT;SIGQUIT;SIGCONT;SIGSTOP;SIGTSTP
  - [feat] add ignore mode
  - [feat] add erase to end of line api
  - [feat] add EOF_ to EOF;SIGINT_ to SIGINT auto convert
  - [fix] chry_readline arg3 arg4 change uint32_t to uint16_t
  - [feat] add auto_refresh control
  - [feat] add chry_readline_config.h
  - [fix] line edit add end '\0'
  - [feat] add CONFIG_READLINE_MAX_COMPLETION, argv argl is now saved on the stack
  - [fix] fix sput parameter 2, add const qualifier
  - [feat] add force block mode by chry_readline_block
