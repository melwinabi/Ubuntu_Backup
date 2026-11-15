# Import the external module
import _thread
import math_ops
_thread.start_new_thread(math_ops.run, ())
# Call the functions from utils.py
result1 = math_ops.add_numbers(10, 5)
result2 = math_ops.multiply_numbers(3, 4)

print("Addition:", result1)      # Output: 15
print("Multiplication:", result2) # Output: 12
