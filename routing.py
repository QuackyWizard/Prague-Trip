import openrouteservice
import requests


def make_client(api_key):
    try:
        client = openrouteservice.Client(key=api_key)
        return client
    except Exception as e:
        print("Failed to create client: %s" % e)

def get_coordinates(address, api_key):
    url = f'https://api.openrouteservice.org/geocode/search?text={address}&api_key={api_key}'
    response = requests.get(url)
    data = response.json()
    if data['features']:
        lon = data['features'][0]['geometry']['coordinates'][1]
        lat = data['features'][0]['geometry']['coordinates'][0]
        return lat, lon
    else:
        return None
    

def get_distance_matrix(client, coordinates):
    distances = client.distance_matrix(
        coordinates,
        profile='driving-car',  # or use 'foot-walking' or 'cycling-regular'
        metrics=['distance', 'duration']
    )
    return distances

def get_route(client, coordinates):
    route = client.directions(
        coordinates=coordinates, 
        profile = 'driving-car',
        format = 'geojson'  
    )
    geometry = route['features'][0]['geometry']['coordinates']
    geometry = [(cord[1], cord[0]) for cord in geometry]
    return geometry