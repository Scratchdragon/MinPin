g++ -o minpin main.cpp
chmod +x minpin
mkdir /etc/minpin
cp src/* /etc/minpin/
cp minpin /usr/bin/minpin