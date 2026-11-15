import numpy as np
import pickle 
import streamlit

loaded_model = pickle.load(open('C:\Users\Melwin Abi\OneDrive\Desktop\AI course\Machine Learning\deabtspider.sav', 'rb')
                           
def diabtes_predict (input_data):
    
   input_data_as_numpy_array = np.asarray(input_data)
 
   input_data_reshaped = input_data_as_numpy_array.reshape(1,-1)
 
   prediction = loaded_model.predict(input_data_reshaped)
 

 #  diabtes_predict():
 
   if (prediction[0] == 0):
    return 'The person is not diabetic'
   else:
    return 'The person is diabetic'