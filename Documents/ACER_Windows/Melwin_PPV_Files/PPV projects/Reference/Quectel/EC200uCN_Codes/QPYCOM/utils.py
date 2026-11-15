import request
import ujson

def get_ip_port():
    """ Fetch IP and Port from API """
    try:
        response = request.get("https://api.cenpilot.in/api/CenPilot/Domine")
        url_string = ""
        
        for i in response.content:
            url_string += i

        parsed_data = ujson.loads(url_string)
        ip = parsed_data['IP']
        port = parsed_data['Port']
        return ip, port
    except Exception as e:
        print("Error fetching IP and Port:", e)
        return None, None
