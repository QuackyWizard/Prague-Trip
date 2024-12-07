from solve_tsp import brute_force, nearest_neighbour, calculate_total_duration
import folium
import routing
import os
from dotenv import load_dotenv
import json
import time
def main():
    #api_key = {your api key}
    load_dotenv()  
    API_KEY = os.getenv("API_KEY")
    client = routing.make_client(API_KEY)


    locations = []
    N = 0 # Number of locations

    with open("adresses.txt","r") as f:
        adresses = [l.strip() for l in f]

    # Get coordinates for each address
    for address in adresses:
        coordinates = routing.get_coordinates(address, API_KEY)
        if coordinates:
            locations.append(coordinates)
            N += 1

    # Get distance matrix
    distances = routing.get_distance_matrix(client, locations)['durations']
    
    

    # Save distance matrix to JSON
    with open("output.json", "w") as f:
        json.dump([{"matrix":distances}], f)

    
    colors = ['red','green','blue','purple','orange','darkred','lightred','beige','darkblue','darkgreen','cadetblue','darkpurple']
    algorithms = [nearest_neighbour,brute_force]
    
    for alg in algorithms:
        r,d= alg(distances,N)
        
        m = folium.Map(location=[locations[0][1], locations[0][0]], zoom_start=20)
        for i in range(N):
            c = colors[i%len(colors)]
            route = [locations[r[i]],locations[r[i+1]]]
            folium.Marker(route[0][::-1], popup=f"Location {i}").add_to(m)
            cords = routing.get_route(client,route)
            folium.PolyLine(cords, color=c, weight=3, opacity=0.8).add_to(m)
        
        m.save(f"{alg.__name__}.html")
        print(f"Total duration for {alg.__name__}: {d}")
        with open("output.json", "a") as f:
            json.dump([{"route":r,"total_duration":d}], f)
        
if __name__ == "__main__":
    main()
