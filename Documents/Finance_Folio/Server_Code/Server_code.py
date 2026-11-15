from flask import Flask, jsonify
import yfinance as yf # Python-ல் Yahoo Finance-க்கான Library

app = Flask(__name__)

@app.route('/api/market-rates', methods=['GET'])
def get_rates():
    # Yahoo-வில் இருந்து தங்கம், வெள்ளி, நிஃப்டி ரேட்களை எடுப்பது
    gold = yf.Ticker("GC=F").fast_info['lastPrice']
    silver = yf.Ticker("SI=F").fast_info['lastPrice']
    nifty = yf.Ticker("^NSEI").fast_info['lastPrice']
    
    # JSON வடிவில் மாற்றி அனுப்புவது
    return jsonify({
        "success": True,
        "nifty": nifty,
        "goldUSD": gold,
        "silverUSD": silver
    })

if __name__ == '__main__':
    app.run(port=5000) # Port 5000-ல் சர்வர் ஓடும்
