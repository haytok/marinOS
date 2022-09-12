FROM otmb/h83069f

RUN { \
  echo "alias ll='ls -l'"; \
  echo "alias la='ls -A'"; \
  echo "alias l='ls -CF'"; \
  echo "PS1='\[\e[1;35m\][\u@\h \W]\\$ \[\e[m\]'"; \
  echo "alias a='cd ../'"; \
  echo "alias aa='cd ../../'"; \
  echo "alias aaa='cd ../../../'"; \
  echo "alias aaaa='cd ../../../../'"; \
  echo "alias aaaaa='cd ../../../../../'"; \
  echo "alias marinos='cd /home/haytok/src/'"; \
} > ~/.bashrc
