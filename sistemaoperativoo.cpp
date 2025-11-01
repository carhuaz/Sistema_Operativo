#include <iostream>
#include <cstring>

using namespace std;

// ============================================
// FUNCIONES AUXILIARES
// ============================================

// Convierte todo el texto a minúsculas (más seguro para validaciones)
void convertirMinusculas(char* texto) {
    for (int i = 0; i < strlen(texto); i++) {
        if (texto[i] >= 'A' && texto[i] <= 'Z') {
            texto[i] = texto[i] + 32;
        }
    }
}

// Valida si un estado es válido (todo en minúsculas)
bool esEstadoValido(const char* estado) {
    char temp[20];
    strncpy(temp, estado, 19);
    temp[19] = '\0';
    convertirMinusculas(temp);
    
    return (strcmp(temp, "listo") == 0 || 
            strcmp(temp, "ejecutando") == 0 || 
            strcmp(temp, "terminado") == 0);
}

// ============================================
// ESTRUCTURA: PROCESO
// ============================================
struct Proceso {
    int id;
    char nombre[50];
    int prioridad;
    char estado[20];
    Proceso* siguiente;

    Proceso(int _id, const char* _nombre, int _prioridad, const char* _estado) {
        id = _id;
        strncpy(nombre, _nombre, 49);
        nombre[49] = '\0';
        prioridad = _prioridad;
        strncpy(estado, _estado, 19);
        estado[19] = '\0';
        siguiente = NULL;
    }
};

// ============================================
// CLASE: GESTOR DE PROCESOS
// ============================================
class GestorProcesos {
private:
    Proceso* cabeza;
    int contadorID;

    // Reorganiza IDs consecutivamente
    void reorganizarIDs() {
        int cantidad = contarProcesos();
        if (cantidad == 0) {
            contadorID = 1;
            return;
        }

        Proceso** arreglo = crearArregloProcesos(cantidad);
        ordenarArregloPorID(arreglo, cantidad);
        
        // Asignar IDs consecutivos
        for (int i = 0; i < cantidad; i++) {
            arreglo[i]->id = i + 1;
        }
        
        contadorID = cantidad + 1;
        delete[] arreglo;
    }

    // Cuenta total de procesos en la lista
    int contarProcesos() {
        int cantidad = 0;
        Proceso* temp = cabeza;
        while (temp != NULL) {
            cantidad++;
            temp = temp->siguiente;
        }
        return cantidad;
    }

    // Crea un arreglo dinámico con todos los procesos
    Proceso** crearArregloProcesos(int cantidad) {
        Proceso** arreglo = new Proceso*[cantidad];
        Proceso* temp = cabeza;
        for (int i = 0; i < cantidad; i++) {
            arreglo[i] = temp;
            temp = temp->siguiente;
        }
        return arreglo;
    }

    // Ordena el arreglo por ID usando burbuja
    void ordenarArregloPorID(Proceso** arreglo, int cantidad) {
        for (int i = 0; i < cantidad - 1; i++) {
            for (int j = 0; j < cantidad - i - 1; j++) {
                if (arreglo[j]->id > arreglo[j + 1]->id) {
                    Proceso* aux = arreglo[j];
                    arreglo[j] = arreglo[j + 1];
                    arreglo[j + 1] = aux;
                }
            }
        }
    }

public:
    GestorProcesos() {
        cabeza = NULL;
        contadorID = 1;
    }

    // Inserta proceso con estado inicial "listo"
    void insertar(const char* nombre, int prioridad) {
        Proceso* nuevo = new Proceso(contadorID++, nombre, prioridad, "listo");
        nuevo->siguiente = cabeza;
        cabeza = nuevo;
        cout << "Proceso creado con ID: " << nuevo->id << " (estado: listo)" << endl;
    }

    // Muestra procesos ordenados en tabla
    void mostrar() {
        if (cabeza == NULL) {
            cout << "\n*** No hay procesos en el sistema ***\n";
            return;
        }

        int cantidad = contarProcesos();
        Proceso** arreglo = crearArregloProcesos(cantidad);
        ordenarArregloPorID(arreglo, cantidad);

        cout << "\n=============== LISTA DE PROCESOS ===============\n";
        cout << "ID    Nombre                  Prioridad    Estado\n";
        cout << "=================================================\n";
        
        for (int i = 0; i < cantidad; i++) {
            char buffer[100];
            sprintf(buffer, "%-6d%-24s%-13d%s", 
                    arreglo[i]->id, 
                    arreglo[i]->nombre, 
                    arreglo[i]->prioridad, 
                    arreglo[i]->estado);
            cout << buffer << endl;
        }
        cout << "=================================================\n";

        delete[] arreglo;
    }

    // Busca proceso por ID
    Proceso* buscar(int id) {
        Proceso* actual = cabeza;
        while (actual != NULL) {
            if (actual->id == id) return actual;
            actual = actual->siguiente;
        }
        return NULL;
    }

    // Muestra ficha detallada del proceso
    void mostrarFicha(Proceso* proc) {
        cout << "\n+================================================+\n";
        cout << "|            INFORMACION DEL PROCESO             |\n";
        cout << "+================================================+\n";
        cout << "| ID:              " << proc->id << endl;
        cout << "| Nombre:          " << proc->nombre << endl;
        cout << "| Prioridad:       " << proc->prioridad << " (escala 1-10)" << endl;
        cout << "| Estado:          " << proc->estado << endl;
        cout << "+================================================+\n";
    }

    // Elimina proceso con confirmación
    void eliminar(int id) {
        if (cabeza == NULL) {
            cout << "Error: No hay procesos para eliminar.\n";
            return;
        }

        Proceso* proc = buscar(id);
        if (proc == NULL) {
            cout << "Error: No se encontro un proceso con ID " << id << endl;
            return;
        }

        cout << "\nProceso a eliminar:\n";
        cout << "  ID: " << proc->id << " | Nombre: " << proc->nombre 
             << " | Prioridad: " << proc->prioridad << endl;

        char respuesta[10];
        bool respuestaValida = false;
        
        while (!respuestaValida) {
            cout << "\nEsta seguro que desea eliminar este proceso? (s/n): ";
            cin.getline(respuesta, 10);
            convertirMinusculas(respuesta);
            
            if (strcmp(respuesta, "s") == 0 || strcmp(respuesta, "si") == 0) {
                respuestaValida = true;
                Proceso *actual = cabeza, *anterior = NULL;
                
                while (actual != NULL) {
                    if (actual->id == id) {
                        if (anterior != NULL)
                            anterior->siguiente = actual->siguiente;
                        else
                            cabeza = actual->siguiente;
                        
                        cout << "\nProceso '" << actual->nombre << "' eliminado.\n";
                        delete actual;
                        reorganizarIDs();
                        cout << "Los IDs han sido reorganizados.\n";
                        return;
                    }
                    anterior = actual;
                    actual = actual->siguiente;
                }
            } else if (strcmp(respuesta, "n") == 0 || strcmp(respuesta, "no") == 0) {
                cout << "\nOperacion cancelada.\n";
                return;
            } else {
                cout << "Respuesta invalida. Ingrese 's' o 'n'.\n";
            }
        }
    }

    // Modifica prioridad
    void modificarPrioridad(int id, int nuevaPrioridad) {
        Proceso* proc = buscar(id);
        if (proc != NULL) {
            int prioridadAnterior = proc->prioridad;
            proc->prioridad = nuevaPrioridad;
            cout << "Prioridad actualizada de " << prioridadAnterior 
                 << " a " << nuevaPrioridad << endl;
        } else {
            cout << "Error: No se encontro el proceso.\n";
        }
    }

    // Cambia estado (todo en minúsculas)
    void cambiarEstado(int id, const char* nuevoEstado) {
        Proceso* proc = buscar(id);
        if (proc != NULL) {
            char estadoAnterior[20];
            strcpy(estadoAnterior, proc->estado);
            
            char estadoNormalizado[20];
            strncpy(estadoNormalizado, nuevoEstado, 19);
            estadoNormalizado[19] = '\0';
            convertirMinusculas(estadoNormalizado);
            
            strncpy(proc->estado, estadoNormalizado, 19);
            proc->estado[19] = '\0';
            
            cout << "Estado actualizado de '" << estadoAnterior 
                 << "' a '" << estadoNormalizado << "'\n";
        } else {
            cout << "Error: No se encontro el proceso.\n";
        }
    }

    // Retorna array de procesos ordenado (para uso externo)
    Proceso** obtenerArregloProcesos(int& cantidad) {
        cantidad = contarProcesos();
        if (cantidad == 0) return NULL;
        
        Proceso** arreglo = crearArregloProcesos(cantidad);
        ordenarArregloPorID(arreglo, cantidad);
        return arreglo;
    }

    bool estaVacia() {
        return cabeza == NULL;
    }

    Proceso* obtenerCabeza() {
        return cabeza;
    }

    ~GestorProcesos() {
        Proceso* actual = cabeza;
        while (actual != NULL) {
            Proceso* temp = actual;
            actual = actual->siguiente;
            delete temp;
        }
    }
};

// ============================================
// MÓDULOS PENDIENTES
// ============================================
class GestorMemoria {
public:
    GestorMemoria() {
        cout << "[INFO] Gestor de Memoria inicializado (pendiente)\n";
    }
    void menuMemoria() {
        cout << "\n--- GESTOR DE MEMORIA ---\n";
        cout << "Este modulo sera implementado por el equipo.\n";
        cout << "\nPresione Enter para continuar...";
        cin.get();
    }
};

class PlanificadorCPU {
private:
	// Estructura que representa un proceso 
    struct Nodo {
        Proceso* proceso;	// Puntero al proceso que contiene el nodo
		Nodo* siguiente;	// Puntero al siguiente nodo en la cola		
	// Constructor del nodo
        Nodo(Proceso* p) {
            proceso = p;
            siguiente = NULL;
        }
    };

    Nodo* frente = NULL;   // Primer nodo de la cola de procesos     
    Nodo* fin = NULL;   
	Nodo*ejecutados = NULL; // Lista de procesos que ya fueron ejecutados
 
    		

public:
	// Constructor del planificador
    PlanificadorCPU() {
        frente = NULL; 			// Inicializa la cola vacía
        fin = NULL;
        ejecutados = NULL;		// Inicializa la lista de ejecutados vacía
        cout << "[INFO] Planificador de CPU inicializado correctamente\n";
    }

    // Inserta un proceso en la cola según su prioridad
    void encolar(Proceso* proc) {
    Nodo* nuevo = new Nodo(proc);		// Crear nuevo nodo para el proceso

    // Si la cola está vacía o tiene mayor prioridad
    if (frente == NULL || proc->prioridad > frente->proceso->prioridad) {
        nuevo->siguiente = frente;		// Nuevo nodo apunta al frente actual
        frente = nuevo;					// Nuevo nodo se convierte en el frente
    } else {	
    	// Buscar la posición correcta según prioridad
        Nodo* temp = frente;
        while (temp->siguiente != NULL && temp->siguiente->proceso->prioridad >= proc->prioridad) {
            temp = temp->siguiente;
        }
        nuevo->siguiente = temp->siguiente;	// Insertar nodo en la posición
        temp->siguiente = nuevo;
    }

    cout << "Proceso '" << proc->nombre << "' agregado a la cola (Prioridad: "
         << proc->prioridad << ")" << endl;
}

    // Desencola (ejecuta) el proceso con mayor prioridad
    void ejecutarProceso() {
    if (frente == NULL) {
        cout << "No hay procesos en la cola." << endl;
        return;
    }

    Nodo* temp = frente;		// Tomar el nodo del frente
    frente = frente->siguiente;
	// Cambia el estado del proceso
    strcpy(temp->proceso->estado, "ejecutando");
    cout << "Ejecutando proceso: " << temp->proceso->nombre << endl;

    strcpy(temp->proceso->estado, "terminado");
    cout << "Proceso '" << temp->proceso->nombre << "' ha terminado." << endl;

    // Guardar en historial de ejecutados SIN eliminar
    temp->siguiente = ejecutados;
    ejecutados = temp;
}
    // Muestra los procesos que ya fueron ejecutados
    void mostrarEjecutados() {
    if (ejecutados == NULL) {
        cout << "\n Aun no hay procesos ejecutados.\n";
        return;
    }

    cout << "\n======= HISTORIAL DE PROCESOS EJECUTADOS ======\n";
    cout << "ID    Nombre                  Prioridad\n";
    cout << "===============================================\n";

    Nodo* temp = ejecutados;
    while (temp != NULL) {
        cout << temp->proceso->id;

        // Ajuste de espacios según tamaño del ID
        if (temp->proceso->id < 10) cout << "     ";
        else if (temp->proceso->id < 100) cout << "    ";
        else cout << "   ";

        // Imprimir nombre y ajustar espacios
        cout << temp->proceso->nombre;			// Mostrar nombre del proceso
        int len = strlen(temp->proceso->nombre);
        for (int i = len; i < 24; i++) cout << " ";		// Ajustar espacios

        // Imprimir prioridad
        cout << temp->proceso->prioridad << endl;

        temp = temp->siguiente;
    }

    cout << "===============================================\n";
}

// Permite eliminar procesos ejecutados, individualmente o todos
void eliminarProcesosEjecutados() {
    int opcion;

    do {
        cout << "\n===== ELIMINAR PROCESOS EJECUTADOS =====\n";
        cout << "1. Eliminar por ID\n";
        cout << "2. Eliminar todos los procesos ejecutados\n";
        cout << "3. Volver al menu del planificador\n";
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        // Validación  de entrada
        if (cin.fail()) {
            cin.clear();                // Limpiar error de entrada
            cin.ignore(1000, '\n');     
            cout << "\nIngrese solo numeros validos.\n";
        } else {
            // Seleccionar acción según opción ingresada
            switch (opcion) {

                case 1: { // Eliminar proceso por ID
                    if (ejecutados == NULL) {
                        // Si esta vacía
                        cout << "\nNo hay procesos ejecutados.\n";
                    } else {
                        int id;
                        cout << "Ingrese el ID del proceso a eliminar: ";
                        cin >> id;

                        if (cin.fail()) {
                            cin.clear();
                            cin.ignore(1000, '\n');
                            cout << "\nIngrese un ID valido (solo numeros).\n";
                        } else {
                            Nodo* temp = ejecutados;
                            Nodo* anterior = NULL;
                            bool encontrado = false;

                            // Buscar nodo con ID especificado
                            while (temp != NULL) {
                                if (temp->proceso->id == id) {
                                    encontrado = true;
                                    // Si es el primer nodo de la lista
                                    if (anterior == NULL)
                                        ejecutados = temp->siguiente;
                                    else
                                        anterior->siguiente = temp->siguiente;

                                    delete temp; // Liberar memoria del nodo
                                    cout << "\nProceso con ID " << id << " eliminado del historial.\n";
                                    break; 
                                }
                                anterior = temp;
                                temp = temp->siguiente;
                            }

                            // Si no se encontró el ID
                            if (!encontrado)
                                cout << "\nNo se encontro ningun proceso con ese ID.\n";
                        }
                    }
                    break; 
                }

                case 2: { // Eliminar todos los procesos ejecutados
                    if (ejecutados == NULL) {
                        cout << "\nNo hay procesos ejecutados.\n";
                    } else {
                        Nodo* temp = ejecutados;
                        // Recorrer la lista y liberar memoria de cada nodo
                        while (temp != NULL) {
                            Nodo* borrar = temp;
                            temp = temp->siguiente;
                            delete borrar;
                        }
                        ejecutados = NULL; // Lista queda vacía
                        cout << "\nTodos los procesos ejecutados han sido eliminados.\n";
                    }
                    break; 
                }

                case 3: // Salir del menú
                    cout << "\nRegresando al menu del planificador...\n";
                    break;

                default: // Opción inválida
                    cout << "\nOpcion no valida.\n";
                    break;
            }
        }

// Repetir mientras la opción no sea 3
    } while (opcion != 3);
}

    // Muestra la cola actual
    void mostrarCola() {
    if (frente == NULL) {
        cout << "\n*** La cola de procesos esta vacia ***\n";
        return;
    }

    cout << "\n=============== COLA DE PROCESOS ===============\n";
    cout << "ID    Nombre                  Prioridad    Estado\n";
    cout << "=================================================\n";

    Nodo* temp = frente;
    while (temp != NULL) {
        cout << temp->proceso->id;

        // Espacios según tamaño del ID
        if (temp->proceso->id < 10) cout << "     ";
        else if (temp->proceso->id < 100) cout << "    ";
        else cout << "   ";

        cout << temp->proceso->nombre;
        int len = strlen(temp->proceso->nombre);

        // Ajuste de espacios para nombre
        for (int i = len; i < 24; i++) cout << " ";

        cout << temp->proceso->prioridad;
        if (temp->proceso->prioridad < 10) cout << "           ";
        else if (temp->proceso->prioridad < 100) cout << "          ";
        else cout << "         ";

        cout << temp->proceso->estado << endl;

        temp = temp->siguiente;
    }

    cout << "=================================================\n";
}
    // Menú del planificador
    void menuPlanificador(GestorProcesos& gestor) {
        int opcion;
        string nombre;
        int prioridad;

        do {
            cout << "\n======= PLANIFICADOR DE CPU =======\n";
            cout << "1. Encolar proceso\n";
            cout << "2. Ejecutar proceso (desencolar)\n";
            cout << "3. Mostrar cola actual\n";
            cout << "4. Mostrar procesos ejecutados\n";
            cout << "5. Eliminar procesos ejecutados\n";
            cout << "6. Volver al menu principal\n";
            cout << "===================================\n";
            cout << "Opcion: ";
            cin >> opcion;
            cin.ignore();

            switch (opcion) {
               case 1: {		// Encolar proceso seleccionado por el usuario
    			if (gestor.estaVacia()) {
        		cout << "\nNo hay procesos creados en el Gestor de Procesos.\n";	// Verificar si hay procesos en el gestor
        		break;
    				}

    			int idSeleccionado;		// ID del proceso que desea encolar
    			Proceso* p = NULL;		// Puntero al proceso seleccionado, inicializado en NULL
    			gestor.mostrar();  		// Mostrar todos los procesos disponibles

   				bool idValido = false;	
    			while (!idValido) {		// Bucle para asegurar que se ingrese un ID válido
        			cout << "\nIngrese el ID del proceso que desea encolar: ";
        			cin >> idSeleccionado;

        		if (cin.fail()) {		// Si la entrada no es un número
            		cin.clear();
            		cin.ignore(1000, '\n');
            		cout << "Ingrese un numero valido (solo digitos).\n";
        		}
        		else if (idSeleccionado <= 0) {  // Si el número no es positivo
            		cout << "El ID debe ser un numero positivo.\n";
        		}
        		else {
            		p = gestor.buscar(idSeleccionado);		// Buscar proceso por ID
            		if (p == NULL) {
                		cin.ignore(1000, '\n');
                		cout << "No existe ningun proceso con ese ID.\n";
            	}
            	else {  // Si se encontró el proceso
                	cin.ignore(1000, '\n'); 
                	idValido = true;       // ID válido, salir del bucle 
            }
        }
    }   			
    			if (p != NULL) {
        			Nodo* temp = frente;	// Nodo temporal para recorrer la cola
        			bool yaEncolado = false;	// Detectar si ya está encolado

        		while (temp != NULL) {		// Recorrer la cola de procesos
            		if (temp->proceso == p) {   	// Si se encuentra el proceso en la cola
                		yaEncolado = true;		// Marcar que ya fue encolado
                		break;
            		}
            		temp = temp->siguiente;	// Avanzar al siguiente nodo
        		}

        		if (yaEncolado) {  // Si ya estaba en la cola
            		cout << "El proceso ya fue encolado anteriormente.\n";
        		}  
        		else {   		// Si no estaba en la cola
            		encolar(p);	 // Encolar el proceso en la cola de CPU
            		strcpy(p->estado, "listo");
            		cout << "Proceso encolado correctamente.\n";
        	}
    	}

    			break;
			}
                case 2:		// Ejecutar proceso (desencolar)
                    ejecutarProceso();
                    break;
                case 3:		// Mostrar la cola actual
                    mostrarCola();
                    break;
                case 4:		// Mostrar los procesos ejecutados
                    mostrarEjecutados();
                    break;
                case 5:		// Eliminar procesos ejecutados
                    eliminarProcesosEjecutados();
                	break;
                case 6:		// Volver al menú principal
                cout << "Volviendo al menu principal...\n";
                	break;
                default:
                    cout << "Opcion invalida.\n";
            }
        } while (opcion != 6);
    }
};

// ============================================
// MENÚ DEL GESTOR DE PROCESOS
// ============================================
void menuGestorProcesos(GestorProcesos& gestor) {
    int opcion, id, prioridad;
    char nombre[50], estado[20];
    bool entradaValida;
    
    do {
        cout << "\n======= GESTOR DE PROCESOS =======\n";
        cout << "1. Agregar proceso\n";
        cout << "2. Mostrar lista de procesos\n";
        cout << "3. Buscar proceso por ID\n";
        cout << "4. Modificar prioridad\n";
        cout << "5. Eliminar proceso\n";
        cout << "6. Cambiar estado\n";
        cout << "7. Volver al menu principal\n";
        cout << "==================================\n";
        cout << "Opcion: ";
        cin >> opcion;
        
        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Error: Ingrese un numero valido.\n";
            continue;
        }
        cin.ignore();

        switch(opcion) {
            case 1: // AGREGAR
                cout << "\n--- AGREGAR NUEVO PROCESO ---\n";
                cout << "Nombre del proceso: ";
                cin.getline(nombre, 50);
                while (strlen(nombre) == 0) {
                    cout << "Error: El nombre no puede estar vacio.\n";
                    cout << "Por favor, ingrese el nombre del proceso: ";
                    cin.getline(nombre, 50);
                }
                
                entradaValida = false;
                while (!entradaValida) {
                    cout << "Prioridad (1-10): ";
                    cin >> prioridad;
                    if (cin.fail()) {
                        cin.clear();
                        cin.ignore(1000, '\n');
                        cout << "Error: Debe ingresar un numero.\n";
                        cout << "Por favor, inserte una prioridad valida (1-10): ";
                    } else if (prioridad < 1 || prioridad > 10) {
                        cin.ignore(1000, '\n');
                        cout << "Error: La prioridad debe estar entre 1 y 10.\n";
                        cout << "Por favor, inserte una prioridad valida: ";
                    } else {
                        entradaValida = true;
                        cin.ignore(1000, '\n');
                    }
                }
                gestor.insertar(nombre, prioridad);
                break;

            case 2: // MOSTRAR
                gestor.mostrar();
                break;

            case 3: // BUSCAR
                if (gestor.estaVacia()) {
                    cout << "Error: No hay procesos en el sistema.\n";
                    break;
                }
                entradaValida = false;
                while (!entradaValida) {
                    cout << "ID del proceso a buscar: ";
                    cin >> id;
                    if (cin.fail()) {
                        cin.clear();
                        cin.ignore(1000, '\n');
                        cout << "Error: El ID debe ser un numero.\n";
                    } else {
                        entradaValida = true;
                        cin.ignore(1000, '\n');
                    }
                }
                {
                    Proceso* encontrado = gestor.buscar(id);
                    if (encontrado != NULL) {
                        gestor.mostrarFicha(encontrado);
                    } else {
                        cout << "\n>>> Proceso no encontrado <<<\n";
                    }
                }
                break;

            case 4: // MODIFICAR PRIORIDAD
                if (gestor.estaVacia()) {
                    cout << "Error: No hay procesos en el sistema.\n";
                    break;
                }
                entradaValida = false;
                while (!entradaValida) {
                    cout << "ID del proceso: ";
                    cin >> id;
                    if (cin.fail()) {
                        cin.clear();
                        cin.ignore(1000, '\n');
                        cout << "Error: El ID debe ser un numero.\n";
                    } else {
                        entradaValida = true;
                        cin.ignore(1000, '\n');
                    }
                }
                if (gestor.buscar(id) == NULL) {
                    cout << "Error: No se encontro el proceso con ID " << id << endl;
                    break;
                }
                entradaValida = false;
                while (!entradaValida) {
                    cout << "Nueva prioridad (1-10): ";
                    cin >> prioridad;
                    if (cin.fail()) {
                        cin.clear();
                        cin.ignore(1000, '\n');
                        cout << "Error: Debe ingresar un numero.\n";
                    } else if (prioridad < 1 || prioridad > 10) {
                        cin.ignore(1000, '\n');
                        cout << "Error: La prioridad debe estar entre 1 y 10.\n";
                    } else {
                        entradaValida = true;
                        cin.ignore(1000, '\n');
                    }
                }
                gestor.modificarPrioridad(id, prioridad);
                break;

            case 5: // ELIMINAR
                if (gestor.estaVacia()) {
                    cout << "Error: No hay procesos para eliminar.\n";
                    break;
                }
                entradaValida = false;
                while (!entradaValida) {
                    cout << "ID del proceso a eliminar: ";
                    cin >> id;
                    if (cin.fail()) {
                        cin.clear();
                        cin.ignore(1000, '\n');
                        cout << "Error: El ID debe ser un numero.\n";
                    } else {
                        entradaValida = true;
                        cin.ignore(1000, '\n');
                    }
                }
                gestor.eliminar(id);
                break;

            case 6: // CAMBIAR ESTADO
                if (gestor.estaVacia()) {
                    cout << "Error: No hay procesos en el sistema.\n";
                    break;
                }
                entradaValida = false;
                while (!entradaValida) {
                    cout << "ID del proceso: ";
                    cin >> id;
                    if (cin.fail()) {
                        cin.clear();
                        cin.ignore(1000, '\n');
                        cout << "Error: El ID debe ser un numero.\n";
                    } else {
                        entradaValida = true;
                        cin.ignore(1000, '\n');
                    }
                }
                if (gestor.buscar(id) == NULL) {
                    cout << "Error: No se encontro el proceso.\n";
                    break;
                }
                entradaValida = false;
                while (!entradaValida) {
                    cout << "Nuevo estado (listo/ejecutando/terminado): ";
                    cin.getline(estado, 20);
                    convertirMinusculas(estado);
                    
                    if (strlen(estado) == 0) {
                        cout << "Error: El estado no puede estar vacio.\n";
                    } else if (!esEstadoValido(estado)) {
                        cout << "Error: Estado invalido.\n";
                        cout << "Estados validos: listo, ejecutando, terminado\n";
                    } else {
                        entradaValida = true;
                    }
                }
                gestor.cambiarEstado(id, estado);
                break;

            case 7:
                cout << "Volviendo al menu principal...\n";
                break;

            default:
                cout << "Opcion invalida.\n";
        }
    } while(opcion != 7);
}

// ============================================
// MAIN
// ============================================
int main() {
    GestorProcesos gestor;
    GestorMemoria memoria;
    PlanificadorCPU planificador;
    int opcion;

    do {
        cout << "\n========================================\n";
        cout << "   SISTEMA DE GESTION DE PROCESOS\n";
        cout << "========================================\n";
        cout << "1. Gestor de Procesos [FUNCIONAL]\n";
        cout << "2. Gestor de Memoria [PENDIENTE]\n";
        cout << "3. Planificador de CPU [FUNCIONAL]\n";
        cout << "4. Salir\n";
        cout << "========================================\n";
        cout << "Seleccione modulo: ";
        cin >> opcion;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Error: Ingrese un numero valido.\n";
            continue;
        }

        switch(opcion) {
            case 1:
                menuGestorProcesos(gestor);
                break;
            case 2:
                memoria.menuMemoria();
                break;
            case 3:
                planificador.menuPlanificador(gestor);
                break;
            case 4:
                cout << "\nGracias por usar el sistema!\n";
                break;
            default:
                cout << "Opcion invalida.\n";
        }
    } while(opcion != 4);

    return 0;
}

