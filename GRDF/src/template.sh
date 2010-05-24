#!/bin/sh

camelscore=$1
lowerscore=`echo $camelscore | tr '[A-Z]' '[a-z]'`
upperscore=`echo $camelscore | tr '[a-z]' '[A-Z]'`

camel=`echo $camelscore | sed 's/_//g'`
lower=`echo $camel | tr '[A-Z]' '[a-z]'`
upper=`echo $camel | tr '[a-z]' '[A-Z]'`

lowerdash=`echo $lowerscore | tr '_' '-'`

for n in c h;do
  cat grdf-node-anon.$n | \
    sed -e "s/_node_anon/_$lowerscore/g" -e "s/_NODE_ANON/_$upperscore/g" | \
    sed -e "s/node_anon/$lower/g" -e "s/NodeAnon/$camel/g" -e "s/NODEANON/$upper/g" > \
    grdf-$lowerdash.$n
done
