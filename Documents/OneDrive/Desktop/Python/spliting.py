#using .split we can split the portion of a sentence

filename = input("Input the Filename: ") #photo.jpg

f_extns = filename.split(".")


print(f_extns[1])

#..................................................................

txt = "welcome to the jungle"

x = txt.split()

print(x)
