from bottle import Bottle, run, request, response
import requests
from datetime import datetime

app = Bottle()

# Replace with your currency API key
API_KEY = "b5ddaa4f0cfce759d2bdd72f"
BASE_URL = f"https://v6.exchangerate-api.com/v6/{API_KEY}/latest"

@app.route('/exchange_rate', method='GET')
def get_exchange_rate():
    base_currency = request.query.base_currency or "USD"
    target_currency = request.query.target_currency or "EUR"
    
    # Request exchange rate data from the currency API
    url = f"{BASE_URL}/{base_currency}"
    api_response = requests.get(url)

    # Check if the request was successful
    if api_response.status_code == 200:
        data = api_response.json()
        # Prepare the response with the current time
        if "conversion_rates" in data and target_currency in data["conversion_rates"]:
            exchange_rate = data["conversion_rates"][target_currency]
            current_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            response.content_type = 'application/json'
            return {
                "base_currency": base_currency,
                "target_currency": target_currency,
                "rate": exchange_rate,
                "timestamp": current_time  # Add the current time to the response
            }
        else:
            response.status = 404
            return {"error": "Exchange rate not found"}
    else:
        response.status = 500
        return {"error": "Failed to retrieve data from currency API"}

# Run the Bottle server
if __name__ == "__main__":
    run(app, host="0.0.0.0", port=8080)

