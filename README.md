# COA-Project
PREREQUISITES:
app.py
- install bottle if you haven't do so. (pip install bottle) 
- in the code, replace with your currency API key and base URL, in my case i used it from https://www.exchangerate-api.com/ (API_KEY/BASE_URL)
- change base_currency and  target_currency to your preferred one!

Currency_Wifi.ino
- make sure you install these libraries in Arduino.
  1) TinyScreen
  2) WiFi101
  3) ArduinoJson

  - enter your wifi name under ssid
  - enter your wifi password under password
  - enter your wifi ip address in serverUrl
  how to check ip address?
On Windows
Open Command Prompt: Press Win + R, type cmd, and hit Enter.
Run the Command: Type ipconfig and press Enter.
Locate Your IP Address:
Look for the network interface you’re using (e.g., "Wi-Fi" or "Ethernet").
Find the line labelled IPv4 Address—that’s your local IP address. It will look something like 192.168.1.5.

On macOS
Open System Preferences: Go to Apple Menu > System Preferences > Network.
Select Your Network Connection: Choose the active network (e.g., Wi-Fi or Ethernet).
Find the IP Address: You should see your IP address displayed in the details. It will look something like 192.168.1.5.
Alternatively, you can also use the Terminal:
Open Terminal.
Type ifconfig and press Enter.
Look for the inet entry under your active network interface (often en0 for Wi-Fi). The address next to inet is your IP.




---------------------------------------------------------------------------------------------------------------------------------------------------

HOW TO RUN THE WEBSITE:
- From the terminal, run "python app.py"
- Go to chrome and type "http://localhost:8080/exchange_rate"

WHEN UPLOADING Currency_Wifi.ino
- ensure you have connected TinyZero, Wifi Board and TinyScreen.

- Laptop and Wifi Board are connected to the same Wifi.

- can check tools > serial monitor if there are any errors during uploading.

- data will be updated to the tinyscreen every min.
