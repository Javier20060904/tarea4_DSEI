from tkinter import Tk, Frame, Button, Label, ttk
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import matplotlib.pyplot as plt 
from datetime import datetime
import serial
import re

# Configuración del puerto serial
puerto_serial = serial.Serial('COM3', 9600, timeout=1)  # Cambia 'COM3' por el puerto correcto

# Expresiones regulares para extraer los datos de los sensores
estructura_temp = re.compile(r'Sensor(\d+): ([\d.]+) C')  # Para capturar valores de temperatura
estructura_no_disponible = re.compile(r'Sensor(\d+): N/D')  # Para capturar "N/D"

# Inicializar diccionario para almacenar los valores de los sensores
lecturas_sensores = {"1": None, "2": None, "3": None}

# Variables para almacenar los datos de cada sensor
valor_sensor1 = None
valor_sensor2 = None
valor_sensor3 = None

# Crear la figura y los ejes para la gráfica
fig, ax = plt.subplots(dpi=90, figsize=(7, 5), facecolor='#f5f5f5')
plt.title("Gráfica de temperatura", color='black', size=16, family="Arial")

# Configurar límites iniciales y colores de la gráfica
plt.xlim(0, 10)
ax.set_facecolor('#eaeaea')

ax.axhline(linewidth=2, color='black')
ax.axvline(linewidth=2, color='black')

ax.set_xlabel("Tiempo", color='black')
ax.set_ylabel("Temperatura", color='black')
ax.tick_params(direction='out', length=6, width=2, colors='black', grid_color='black', grid_alpha=0.5)

# Configuración inicial de datos
t = []
y_temp1 = []
y_temp2 = []
y_temp3 = []

# Variables para la gráfica
num_sensor = "sensor1"
inicio_tiempo = datetime.now()

# Función para graficar valores aleatorios y actualizar la gráfica
def graficar_datos():
    global t, y_temp1, y_temp2, y_temp3, num_sensor
    global lecturas_sensores
    # Generar nuevo punto de datos
    tiempo_actual = (datetime.now() - inicio_tiempo).total_seconds()
    
    # Generar valores aleatorios para las gráficas
    while not sensores_completos(lecturas_sensores):
        linea = puerto_serial.readline().decode('utf-8').strip()  # Leer una línea del puerto serial
        
        # Intentar buscar un valor de temperatura
        medicion_temp = estructura_temp.search(linea)
        if medicion_temp:
            sensor_id = medicion_temp.group(1)
            val_temp = medicion_temp.group(2)
            lecturas_sensores[sensor_id] = f"{val_temp}"  # Guardar la lectura en el diccionario
        
        # Intentar buscar si algún sensor no tiene datos
        no_disponible = estructura_no_disponible.search(linea)
        if no_disponible:
            sensor_id = no_disponible.group(1)
            lecturas_sensores[sensor_id] = "N/D"  # Guardar "N/D" si no hay datos
    
    # Asignar los valores a las variables específicas
    if lecturas_sensores.get("1") == "N/D":
        val_sensor1 = 0
    else:
        val_sensor1 = float(lecturas_sensores.get("1"))
    if lecturas_sensores.get("2") == "N/D":
        val_sensor2 = 0
    else:
        val_sensor2 = float(lecturas_sensores.get("2"))
    if lecturas_sensores.get("3") == "N/D":
        val_sensor3 = 0
    else:
        val_sensor3 = float(lecturas_sensores.get("3"))
    t.append(tiempo_actual)
    

    y_temp1.append(val_sensor1)
    y_temp2.append(val_sensor2)  
    y_temp3.append(val_sensor3)
    
    lecturas_sensores = {"1": None, "2": None, "3": None}
    
    # Limpiar la gráfica antes de dibujar una nueva
    ax.clear()
    ax.set_facecolor('#eaeaea')
    ax.axhline(linewidth=2, color='black')
    ax.axvline(linewidth=2, color='black')
    ax.set_xlabel("Tiempo (s)", color='black')
    ax.set_ylabel("Temperatura", color='black')
    ax.tick_params(direction='out', length=6, width=2, colors='black', grid_color='black', grid_alpha=0.5)
    
    # Ajustar los límites del eje X y Y
    ax.set_xlim(0, max(t) if t else 10)
    ax.set_ylim(35, 50)
    
    # Graficar los datos
    if num_sensor == "sensor1":
        ax.plot(t, y_temp1, color='royalblue', linestyle='solid', label="Sensor1")
    elif num_sensor == "sensor2":
        ax.plot(t, y_temp2, color='darkgreen', linestyle='solid', label="Sensor2")
    elif num_sensor == "sensor3":
        ax.plot(t, y_temp3, color='darkorange', linestyle='solid', label="Sensor3")
    
    ax.legend()  # Mostrar la leyenda de las gráficas
    canvas.draw()
    
    # Actualizar la gráfica periódicamente
    ventana.after(100, graficar_datos)  # Actualiza cada segundo

    actualizar_labels(val_sensor1, val_sensor2, val_sensor3)

def actualizar_labels(val_sensor1, val_sensor2, val_sensor3):
    hora_actual = datetime.now().strftime('%H:%M:%S')
    label_hora.config(text=f'Hora: {hora_actual}')
    if val_sensor1:
        label_temp1.config(text=f'Temperatura: {val_sensor1}')
    else:
        label_temp1.config(text=f'Temperatura: N/D')
    if val_sensor2:
        label_temp2.config(text=f'Temperatura: {val_sensor2}')
    else: 
        label_temp2.config(text=f'Temperatura: N/D')
    if val_sensor3:
        label_temp3.config(text=f'Temperatura: {val_sensor3}')
    else:
        label_temp3.config(text=f'Temperatura: N/D')
# Funciones para cambiar entre graficas
def graficar_sensor1():
    global num_sensor
    num_sensor = "sensor1"

def graficar_sensor2():
    global num_sensor
    num_sensor = "sensor2"

def graficar_sensor3():
    global num_sensor
    num_sensor = "sensor3"

# Función para verificar si ya se tienen los datos de los 3 sensores
def sensores_completos(lecturas):
    return all(lectura is not None for lectura in lecturas.values())

# Configuración de la ventana y los widgets
ventana = Tk()
ventana.geometry('675x675')
ventana.wm_title('Gráfica de temperatura')
ventana.minsize(width=675, height=675)

# Frame para los labels
frame_labels = Frame(ventana, bg='#e0e0e0', bd=3)
frame_labels.grid(column=2, row=0, columnspan=4, padx=10, pady=10, sticky='ew')

# Labels en el frame superior
label_hora = Label(frame_labels, text='Hora: --:--:--', bg='#e0e0e0', fg='black')
label_hora.grid(column=0, row=1, padx=5, pady=5)

label_temp1 = Label(frame_labels, text='N/D', bg='#e0e0e0', fg='black')
label_temp1.grid(column=1, row=0, padx=5, pady=5)

label_temp2 = Label(frame_labels, text='N/D', bg='#e0e0e0', fg='black')
label_temp2.grid(column=1, row=1, padx=5, pady=5)

label_temp3 = Label(frame_labels, text='N/D', bg='#e0e0e0', fg='black')
label_temp3.grid(column=1, row=2, padx=5, pady=5)

label_funcion = Label(frame_labels, text='Sensor 1', bg='#e0e0e0', fg='black')
label_funcion.grid(column=2, row=0, padx=5, pady=5)

label_funcion = Label(frame_labels, text='Sensor 2', bg='#e0e0e0', fg='black')
label_funcion.grid(column=2, row=1, padx=5, pady=5)

label_funcion = Label(frame_labels, text='Sensor 3', bg='#e0e0e0', fg='black')
label_funcion.grid(column=2, row=2, padx=5, pady=5)


# Crear el área de dibujo en Tkinter
frame_canvas = Frame(ventana, bg='#e0e0e0', bd=3)
frame_canvas.grid(column=0, row=1, columnspan=4, padx=10, pady=10, sticky='nsew')

canvas = FigureCanvasTkAgg(fig, master=frame_canvas)
canvas.get_tk_widget().grid(column=0, row=0, columnspan=4, padx=5, pady=5)

Button(frame_canvas, text='Graficar Datos', width=15, bg='mediumpurple', fg='white', command=graficar_datos).grid(column=0, row=1, padx=5, pady=5)

Button(frame_canvas, text='Sensor 1', width=15, bg='deepskyblue', fg='white', command=graficar_sensor1).grid(column=1, row=1, padx=5, pady=5)

Button(frame_canvas, text='Sensor 2', width=15, bg='mediumseagreen', fg='white', command=graficar_sensor2).grid(column=2, row=1, padx=5, pady=5)

Button(frame_canvas, text='Sensor 3', width=15, bg='darkorange', fg='white', command=graficar_sensor3).grid(column=3, row=1, padx=5, pady=5)

style = ttk.Style()
style.configure("Horizontal.TScale", background='#e0e0e0')

ventana.mainloop()