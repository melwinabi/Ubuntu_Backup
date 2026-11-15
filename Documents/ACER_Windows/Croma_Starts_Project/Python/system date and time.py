# Import the 'datetime' module to work with date and time
import datetime

# Get the current date and time
result = datetime.datetime.now()

# Create a datetime object representing the current date and time

# Display a message indicating what is being printed
print("Current date and time : ")

# Print the current date and time in a specific format
print(result.strftime("%d-%m-%y %H:%M:%S"))

# Use the 'strftime' method to format the datetime object as a string with the desired format
