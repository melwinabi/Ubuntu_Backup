import requests

API_KEY = "AIzaSyDNOWV4R2GYU0S_h-fCRtKZfDeDkAUHj0Y"  # Replace with your Google API key

data = {
    "cellTowers": [
        {
            "mobileCountryCode": 404,   # MCC (India)
            "mobileNetworkCode": 40,    # MNC (Airtel)
            "locationAreaCode": 42139,  # LAC
            "cellId": 167563274         # CID
        }
    ]
}

response = requests.post(
    f"https://www.googleapis.com/geolocation/v1/geolocate?key={API_KEY}",
    json=data
)

print(response.json())

