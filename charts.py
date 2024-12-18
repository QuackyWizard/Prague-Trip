import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns
import os
import json


#This program reads the output files from the output directory and create a chart for each algorithm (it's not very pretty, but it works)
def main():

    algorithms = ['Held-Karp', 'Nearest Neighbor', 'Brute Force', 'Ant Colony Optimization']
    df = pd.DataFrame(columns=['num_locations']+algorithms)
   
    for file in os.listdir('output'):
        if file.endswith('.json'):
            with open(f"output/{file}", 'r') as f:
                data = json.load(f)
            
            if 'Held-Karp' in data:
                held_karp = data['Held-Karp']["time"]
            else:
                held_karp = pd.NA

            if 'Nearest Neighbor' in data:
                nearest_neighbour = data['Nearest Neighbor']["time"]
            else:
                nearest_neighbour = pd.NA
            
            if 'Brute Force' in data:
                bruteforce = data['Brute Force']["time"]
            else:
                bruteforce = pd.NA
           
            if 'Ant Colony Optimization' in data:
                aco = data['Ant Colony Optimization']["time"]
            else:
                aco = pd.NA
            num_locations = data['Nearest Neighbor']["size"]

            df.loc[len(df)] = [num_locations, held_karp, nearest_neighbour, bruteforce, aco]
            df.sort_values(by=['num_locations'], inplace=True)
   
    os.makedirs("charts", exist_ok=True)

    
    sns.set_theme(style="whitegrid")

    for alg in algorithms:
        
        if alg == "Nearest Neighbor":
            df2 = df[df['num_locations'] >= 500]
            sns.lineplot(data=df2, x='num_locations', y=alg, label=alg)
        else:    
            sns.lineplot(data=df, x='num_locations', y=alg, label=alg)
        
        plt.figure(figsize=(10, 6))  
        sns.lineplot(data=df, x='num_locations', y=alg, label=alg)
    
        plt.title(f"Execution Time for {alg}")
        plt.xlabel("Number of Locations")
        plt.ylabel("Time (ms)")
        if alg == "Held-Karp":
            plt.xticks(range(5,25))

        
        plt.tight_layout()

        plt.savefig(f'charts/{alg}_chart.png')
        plt.close()  
        
        new_df = df[['num_locations', alg]]
        new_df.to_csv(f'charts/{alg}_data.csv', index=False)



if __name__ == "__main__":
    main()