from solve_tsp import *
import folium
import routing
import os
from dotenv import load_dotenv
import sys
#This program reads a list of addresses from "adresses.txt" file and calculates the shortest route to visit all of them using different algorithms
#The output is saved in a html file for each algorithm
#The program uses the openrouteservice API to get the coordinates and the distance matrix for the locations
#The program uses the solve_tsp.py to calculate the shortest route using different algorithms
#The program uses the folium library to create the map and display the route
#The program uses the dotenv library to load the API key from a .env file

def main(args):
    load_dotenv()  
    API_KEY = os.getenv("API_KEY")
    #API_KEY = {your api key}
    client = routing.make_client(API_KEY)


    locations = []
    N = 0 # Number of locations
    
    if len(args) != 2:
        print("Usage: python main.py <input file path>")
        return 
    try:
        with open(args[1],"r") as f:
            adresses = [l.strip() for l in f]
    except:
        print("Error reading file - adresses.txt")
        return


    # Get coordinates for each address
    for address in adresses:
        coordinates = routing.get_coordinates(address, API_KEY)
        if coordinates:
            locations.append(coordinates)
            N += 1

    print(locations)
    # Get distance matrix
    distances = routing.get_distance_matrix(client, locations)['durations']
    
    output = {}
   
    colors = ['red','green','blue','purple','orange','darkred','lightred','beige','darkblue','darkgreen','cadetblue','darkpurple']
    algorithms = [nearest_neighbour,brute_force, held_karp, ant_colony_optimization]
    
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
        output[alg.__name__] = {"route":r , "duration": d}

    print(output)
    
    
if __name__ == "__main__":
    main(sys.argv)
