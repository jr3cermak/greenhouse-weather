For the MacOS, getting vim to colorize is not so trivial.  Here I provide
an arduino.vim that should be placed in ${HOME}/.vim/syntax.   In your,
${HOME}/.vimrc, I have the following commands:

I have found every vim is setup differently, so experiment with the
commands below to adjust for your taste.

```
au BufNewFile,BufRead *.ino setlocal ft=arduino
set autoindent
set expandtab
set shiftwidth=2
set softtabstop=2
set smartindent
syntax on
```
