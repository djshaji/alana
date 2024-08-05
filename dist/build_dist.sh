COUNT=`git rev-list HEAD --count`
echo packing amprack $COUNT
tar czf ../../releases/amprack-$COUNT.tar.gz --dereference --hard-dereference amprack/
