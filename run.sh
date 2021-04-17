#!/bin/bash

# Make changes
make all
if [[ $? -ne 0 ]]
then
    exit 1
fi

# If not in TMUX break
if ! { [ "$TERM" = "screen" ] || [ -n "$TMUX" ]; } then
    echo "Please run inside tmux"
  tmux new-session ./run.sh $@
  exit 0
fi

numPanes=`tmux list-panes | wc -l`
if [[ $numPanes -lt 2 ]]
then
    tmux split-window -h\; last-pane
fi

paneId=`tmux list-panes | sed '/active/d' | cut -d: -f1`

tmux send-keys -t $paneId "./compute" Enter

sleep 0.5

./lsm $@