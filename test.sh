echo "Small"
echo "time(ms);resMem(kb);shared(kb);priv(kb);rootHash" > results/smallBlake.csv
for i in 1 2 3 4 5 6
do
 echo "run $i"
 ./fsCheckDaemon 2>> results/smallBlake.csv
done

sed -i 's/=BLAKE3/=SHA256/g' fsCheck.config

echo "time(ms);resMem(kb);shared(kb);priv(kb);rootHash" > results/smallSHA256.csv
for i in 1 2 3 4 5 6
do
 echo "run $i"
 ./fsCheckDaemon 2>> results/smallSHA256.csv
done

sed -i 's/=SHA256/=BLAKE3/g' fsCheck.config

echo "Intermediate"
sed -i 's/small/intermediate/g' fsCheck.config
echo "time(ms);resMem(kb);shared(kb);priv(kb);rootHash" > results/intBlake.csv
for i in 1 2 3 4 5 6
do
 echo "run $i"
 ./fsCheckDaemon 2>> results/intBlake.csv
done

sed -i 's/=BLAKE3/=SHA256/g' fsCheck.config

echo "time(ms);resMem(kb);shared(kb);priv(kb);rootHash" > results/intSHA256.csv
for i in 1 2 3 4 5 6
do
 echo "run $i"
 ./fsCheckDaemon 2>> results/intSHA256.csv
done

sed -i 's/=SHA256/=BLAKE3/g' fsCheck.config
sed -i 's/intermediate/big/g' fsCheck.config

echo "Big"
echo "time(ms);resMem(kb);shared(kb);priv(kb);rootHash" > results/bigBlake.csv
for i in 1 2 3 4 5 6
do
 echo "run $i"
 ./fsCheckDaemon 2>> results/bigBlake.csv
done

sed -i 's/=BLAKE3/=SHA256/g' fsCheck.config

echo "time(ms);resMem(kb);shared(kb);priv(kb);rootHash" > results/bigSHA256.csv
for i in 1 2 3 4 5 6
do
 echo "run $i"
 ./fsCheckDaemon 2>> results/bigSHA256.csv
done
sed -i 's/=SHA256/=BLAKE3/g' fsCheck.config
sed -i 's/big/small/g' fsCheck.config