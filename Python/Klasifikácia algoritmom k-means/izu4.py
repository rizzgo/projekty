import math
import sys
import copy
        
class Vector:
    
    def __init__(self, x=None, y=None, z=None):
        self.x = x
        self.y = y
        self.z = z

    def equal(self, vector , e):
        try:
            xbool = abs(self.x - vector.x) < 10**-e
            ybool = abs(self.y - vector.y) < 10**-e
            zbool = abs(self.z - vector.z) < 10**-e
            return xbool and ybool and zbool
        except TypeError:
            return False

    def out(self):
        return [self.x,self.y,self.z]

class Array:

    def __init__(self):
        self.vectors = []

    def get_new_w(self):
        w = Vector(0,0,0)
        for vector in self.vectors:
            w.x += vector.x
            w.y += vector.y
            w.z += vector.z
        w.x /= len(self.vectors)
        w.y /= len(self.vectors)    
        w.z /= len(self.vectors)

        return w

    def out(self):
        return [item.out() for item in self.vectors]

def equal(w1,w2,w3,old_w1,old_w2,old_w3, e):
    w1_eq = w1.equal(old_w1, e)
    w2_eq = w2.equal(old_w2, e) 
    w3_eq = w3.equal(old_w3, e) 
    return w1_eq and w2_eq and w3_eq

e = 2

w1 = Vector(-2,4,3)
w2 = Vector(0,-3,1)
w3 = Vector(-2,2,-4)

print("Zadané ťažiská:")
print(f"w1: {w1.out()}")
print(f"w2: {w2.out()}")
print(f"w3: {w3.out()}")

old_w1 = Vector()
old_w2 = Vector()
old_w3 = Vector()

v = Vector()

vectors = sys.stdin.read()
vectors = vectors.split("\n")

while not equal(w1,w2,w3,old_w1,old_w2,old_w3, e):
    
    zero = Array()
    first = Array()
    second = Array()
    
    print("\n\nHľadanie najkratších vzdialeností:")
    for vector in vectors:
        v.x, v.y, v.z = [int(value) for value in vector.split()]
        
        print(f"Pre bod {v.out()} platí:")
        result_1 = math.sqrt((w1.x-v.x)**2 + (w1.y-v.y)**2 + (w1.z-v.z)**2)
        print(f"Vzdialenosť od {w1.out()} je {result_1}")
        result_2 = math.sqrt((w2.x-v.x)**2 + (w2.y-v.y)**2 + (w2.z-v.z)**2)
        print(f"Vzdialenosť od {w2.out()} je {result_2}")
        result_3 = math.sqrt((w3.x-v.x)**2 + (w3.y-v.y)**2 + (w3.z-v.z)**2)
        print(f"Vzdialenosť od {w3.out()} je {result_3}")
        
        array = result_1,result_2,result_3
        shortest = min(array)
        index = array.index(shortest)
        switch = {
            0: w1.out(),
            1: w2.out(),
            2: w3.out(),
        }
        switch_value = switch.get(index)
        print(f"Najkratšia je {shortest} od {switch_value}\n")

        if (index == 0): 
            zero.vectors.append(copy.deepcopy(v))
        elif (index == 1):
            first.vectors.append(copy.deepcopy(v))
        elif (index == 2):
            second.vectors.append(copy.deepcopy(v))


    old_w1 = copy.deepcopy(w1)
    old_w2 = copy.deepcopy(w2)
    old_w3 = copy.deepcopy(w3)
    w1 = zero.get_new_w()
    w2 = first.get_new_w()
    w3 = second.get_new_w()

    print("\nZhluky:")    
    print(f"1: {zero.out()}")
    print(f"2: {first.out()}")    
    print(f"3: {second.out()}\n")

    print("Nové ťažiská:")
    print(f"w1: {w1.out()}")
    print(f"w2: {w2.out()}")
    print(f"w3: {w3.out()}")

print("""
Ťažiská sú rovnaké ako v predchádzajúcom cykle, preto
sú zhluky z tohto cyklu výsledné.
""")