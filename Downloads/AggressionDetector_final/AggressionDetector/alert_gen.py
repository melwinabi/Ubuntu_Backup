import wave
import math
import struct

def generate_alert_sound(filename="alert.wav", duration=1.0, freq_start=400, freq_end=1200):
    sample_rate = 44100
    n_samples = int(sample_rate * duration)
    amplitude = 16000  # -32768 to 32767 for 16-bit audio
    
    with wave.open(filename, 'w') as wav_file:
        wav_file.setnchannels(1)  # Mono
        wav_file.setsampwidth(2)  # 2 bytes per sample (16-bit)
        wav_file.setframerate(sample_rate)
        
        for i in range(n_samples):
            # Modulate frequency to create a siren effect
            t = i / sample_rate
            # 8 Hz siren modulation speed
            modulation = (math.sin(2 * math.pi * 8 * t) + 1) / 2 
            freq = freq_start + (freq_end - freq_start) * modulation
            
            value = int(amplitude * math.sin(2 * math.pi * freq * t))
            data = struct.pack('<h', value)
            wav_file.writeframes(data)
            
    print(f"Generated {filename}")

if __name__ == "__main__":
    generate_alert_sound()
