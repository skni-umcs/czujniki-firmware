pio run -e production
cd ./.pio/build/production
sudo pkill -f "python3 -m http.server 8000"
python3 -m http.server 8000 > logs.txt &
