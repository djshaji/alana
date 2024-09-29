COUNT=`git rev-list HEAD --count`
echo packing amprack $COUNT
echo ../../releases/amprack-$COUNT.tar.gz
tar czf ../../releases/amprack-$COUNT.tar.gz --dereference --hard-dereference amprack/
