FROM otmb/h83069f

RUN { \
  echo "alias ll='ls -l'"; \
  echo "alias la='ls -A'"; \
  echo "alias l='ls -CF'"; \
  echo "PS1='\[\e[1;35m\][\u@\h \W]\\$ \[\e[m\]'"; \
} > ~/.bashrc
