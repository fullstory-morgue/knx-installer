#!/bin/bash

{
bash --dump-po-strings knoppix-installer

for i in $(find modules/ -name "*.bm")
do
  bash --dump-po-strings $i
done

for i in $(find tools/ -type f )
do
  bash --dump-po-strings $i
done

} | msguniq > po/messages.pot
