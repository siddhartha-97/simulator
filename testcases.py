import numpy as np
coordinates = np.random.randint(1,50,size=(100000,2))
with open("nodes.txt","w") as f:
    f.write("\n".join(" ".join(map(str, x)) for x in coordinates)
)
