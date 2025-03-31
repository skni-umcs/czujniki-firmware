pio run -e production
cd ./.pio/build/production
python3 -m http.server 8000 > logs.txt &
