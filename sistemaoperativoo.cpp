#include <iostream>
#include <cstring>

using namespace std;

// ============================================
// FUNCIONES AUXILIARES
// ============================================

// Convierte todo el texto a min�sculas (m�s seguro para validaciones)
void convertirMinusculas(char* texto) {
    for (int i = 0; i < strlen(texto); i++) {
        if (texto[i] >= 'A' && texto[i] <= 'Z') {
            texto[i] = texto[i] + 32;
        }
    }
}

// Valida si un estado es v�lido (todo en min�sculas)
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

    // Crea un arreglo din�mico con todos los procesos
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

    // Elimina proceso con confirmaci�n
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

    // Cambia estado (todo en min�sculas)
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
// GESTOR DE MEMORIA: Implementación con estructura de pila (LIFO)
// Permite asignar y liberar bloques de memoria para procesos
// ============================================

class GestorMemoria {
private:
    // ============================================
    // ESTRUCTURA BLOQUE DE MEMORIA
    // Representa un bloque de memoria asignado a un proceso
    // ============================================
    struct BloqueMemoria {
        int idProceso;              // ID único del proceso dueño del bloque
        char nombreProceso[50];     // Nombre descriptivo del proceso
        int tamanioMB;              // Tamaño del bloque en Megabytes
        BloqueMemoria* siguiente;   // Puntero al siguiente bloque en la pila
        
        // ============================================
        // CONSTRUCTOR DEL BLOQUE DE MEMORIA
        // Inicializa un nuevo bloque con los datos del proceso
        // ============================================
        BloqueMemoria(int _id, const char* _nombre, int _tamanio) {
            idProceso = _id;
            // Copia segura del nombre del proceso
            strncpy(nombreProceso, _nombre, 49);
            nombreProceso[49] = '\0';  // Asegura terminación de cadena
            tamanioMB = _tamanio;
            siguiente = NULL;  // Inicialmente no apunta a ningún bloque
        }
    };
    
    // ============================================
    // VARIABLES PRIVADAS DEL GESTOR
    // ============================================
    BloqueMemoria* tope;        // Puntero al último bloque asignado (top de la pila)
    int memoriaTotal;           // Capacidad total del sistema en MB
    int memoriaUsada;           // Memoria actualmente ocupada
    
public:
    // ============================================
    // CONSTRUCTOR DEL GESTOR DE MEMORIA
    // Inicializa el sistema con 2048 MB (2GB) de memoria total
    // ============================================
    GestorMemoria() {
        tope = NULL;            // Pila vacía al inicio
        memoriaTotal = 2048;    // 2 GB de memoria total del sistema
        memoriaUsada = 0;       // Sin memoria usada inicialmente
        cout << "[INFO] Gestor de Memoria inicializado (Memoria total: " 
             << memoriaTotal << " MB)\n";
    }
    
    // ============================================
    // BUSCAR PROCESO EN LA MEMORIA
    // Recorre la pila buscando un proceso por su ID
    // Retorna el bloque si lo encuentra, NULL si no existe
    // ============================================
    BloqueMemoria* buscarProceso(int idProceso) {
        BloqueMemoria* actual = tope;
        // Recorre toda la pila desde el tope hasta el fondo
        while (actual != NULL) {
            if (actual->idProceso == idProceso) {
                return actual;  // Encontrado
            }
            actual = actual->siguiente;
        }
        return NULL;  // No encontrado
    }
    
    // ============================================
    // ASIGNAR MEMORIA A UN PROCESO (OPERACIÓN PUSH)
    // Agrega un nuevo bloque de memoria al tope de la pila
    // ============================================
    void asignarMemoria(int idProceso, const char* nombreProceso, int tamanioMB) {
        // ============================================
        // VERIFICAR SI EL PROCESO YA TIENE MEMORIA
        // ============================================
        BloqueMemoria* bloqueExistente = buscarProceso(idProceso);
        
        if (bloqueExistente != NULL) {
            // El proceso ya tiene memoria asignada - ofrecer opciones
            cout << "\n[AVISO] El proceso '" << nombreProceso 
                 << "' ya tiene " << bloqueExistente->tamanioMB << " MB asignados.\n";
            cout << "\nOpciones:\n";
            cout << "1. Reemplazar (liberar " << bloqueExistente->tamanioMB 
                 << " MB y asignar " << tamanioMB << " MB)\n";
            cout << "2. Agregar bloque adicional (total: " 
                 << (bloqueExistente->tamanioMB + tamanioMB) << " MB)\n";
            cout << "3. Cancelar operacion\n";
            cout << "Seleccione una opcion: ";
            
            int opcion;
            cin >> opcion;
            
            // Validar entrada del usuario
            if (cin.fail()) {
                cin.clear();
                cin.ignore(1000, '\n');
                cout << "\n[ERROR] Opcion invalida. Operacion cancelada.\n";
                return;
            }
            cin.ignore(1000, '\n');
            
            if (opcion == 1) {
                // REEMPLAZAR: Liberar bloque anterior y asignar nuevo
                liberarPorID(idProceso);
                cout << "\n[INFO] Bloque anterior liberado. Asignando nuevo bloque...\n";
            } else if (opcion == 2) {
                // AGREGAR: Continuar con asignación normal (permite múltiples bloques)
                cout << "\n[INFO] Agregando bloque adicional...\n";
            } else {
                cout << "\n[INFO] Operacion cancelada.\n";
                return;
            }
        }
        
        // ============================================
        // VERIFICAR DISPONIBILIDAD DE MEMORIA
        // ============================================
        if (memoriaUsada + tamanioMB > memoriaTotal) {
            cout << "\n[ERROR] Memoria insuficiente. Disponible: " 
                 << (memoriaTotal - memoriaUsada) << " MB, Solicitado: " 
                 << tamanioMB << " MB\n";
            return;
        }
        
        // ============================================
        // CREAR Y AGREGAR NUEVO BLOQUE (OPERACIÓN PUSH)
        // ============================================
        BloqueMemoria* nuevo = new BloqueMemoria(idProceso, nombreProceso, tamanioMB);
        nuevo->siguiente = tope;    // El nuevo bloque apunta al anterior tope
        tope = nuevo;               // El nuevo bloque se convierte en el tope
        memoriaUsada += tamanioMB;  // Actualizar contador de memoria usada
        
        // Confirmar asignación exitosa
        cout << "\n[OK] Memoria asignada correctamente:\n";
        cout << "     Proceso ID: " << idProceso << " (" << nombreProceso << ")\n";
        cout << "     Tamanio: " << tamanioMB << " MB\n";
        cout << "     Memoria disponible: " << (memoriaTotal - memoriaUsada) << " MB\n";
    }
    
    // ============================================
    // LIBERAR BLOQUE POR ID DE PROCESO
    // Busca y elimina un bloque específico por ID del proceso
    // ============================================
    void liberarPorID(int idProceso) {
        // Verificar si hay bloques en la pila
        if (tope == NULL) {
            cout << "\n[INFO] No hay bloques de memoria asignados.\n";
            return;
        }
        
        BloqueMemoria* actual = tope;
        BloqueMemoria* anterior = NULL;
        
        // ============================================
        // BUSCAR EL BLOQUE POR ID EN LA PILA
        // ============================================
        while (actual != NULL && actual->idProceso != idProceso) {
            anterior = actual;
            actual = actual->siguiente;
        }
        
        // Verificar si se encontró el bloque
        if (actual == NULL) {
            cout << "\n[ERROR] No se encontro memoria asignada al proceso ID " << idProceso << endl;
            return;
        }
        
        // Guardar información para el mensaje de confirmación
        char nombre[50];
        strcpy(nombre, actual->nombreProceso);
        int tamanio = actual->tamanioMB;
        
        // ============================================
        // ELIMINAR EL NODO DE LA PILA
        // ============================================
        if (anterior == NULL) {
            // El bloque a eliminar está en el tope
            tope = actual->siguiente;
        } else {
            // El bloque está en medio o al final de la pila
            anterior->siguiente = actual->siguiente;
        }
        
        // Actualizar memoria usada y liberar el bloque
        memoriaUsada -= tamanio;
        delete actual;
        
        cout << "\n[OK] Memoria liberada correctamente:\n";
        cout << "     Proceso ID: " << idProceso << " (" << nombre << ")\n";
        cout << "     Tamanio liberado: " << tamanio << " MB\n";
        cout << "     Memoria disponible: " << (memoriaTotal - memoriaUsada) << " MB\n";
    }
    
    // ============================================
    // LIBERAR ÚLTIMO BLOQUE ASIGNADO (OPERACIÓN POP)
    // Elimina el bloque del tope de la pila (LIFO)
    // ============================================
    void liberarMemoria() {
        // Verificar si hay bloques en la pila
        if (tope == NULL) {
            cout << "\n[INFO] No hay bloques de memoria asignados.\n";
            return;
        }
        
        // Guardar información del bloque a liberar
        BloqueMemoria* bloqueALiberar = tope;
        int idProceso = bloqueALiberar->idProceso;
        char nombre[50];
        strcpy(nombre, bloqueALiberar->nombreProceso);
        int tamanio = bloqueALiberar->tamanioMB;
        
        // ============================================
        // REALIZAR OPERACIÓN POP
        // ============================================
        tope = tope->siguiente;     // El nuevo tope es el siguiente bloque
        memoriaUsada -= tamanio;    // Actualizar memoria usada
        delete bloqueALiberar;      // Liberar memoria del bloque
        
        cout << "\n[OK] Memoria liberada correctamente:\n";
        cout << "     Proceso ID: " << idProceso << " (" << nombre << ")\n";
        cout << "     Tamanio liberado: " << tamanio << " MB\n";
        cout << "     Memoria disponible: " << (memoriaTotal - memoriaUsada) << " MB\n";
    }
    
    // ============================================
    // MOSTRAR ESTADO ACTUAL DE LA MEMORIA
    // Muestra todos los bloques desde el tope hasta el fondo
    // ============================================
    void mostrarEstadoMemoria() {
        cout << "\n=============== ESTADO DE LA MEMORIA ===============\n";
        cout << "Memoria Total:      " << memoriaTotal << " MB\n";
        cout << "Memoria Usada:      " << memoriaUsada << " MB\n";
        cout << "Memoria Disponible: " << (memoriaTotal - memoriaUsada) << " MB\n";
        cout << "Porcentaje de uso:  " 
             << (memoriaUsada * 100 / memoriaTotal) << "%\n";
        cout << "====================================================\n";
        
        // Verificar si hay bloques asignados
        if (tope == NULL) {
            cout << "\n*** No hay bloques de memoria asignados ***\n";
            cout << "====================================================\n";
            return;
        }
        
        // Mostrar lista de bloques asignados
        cout << "\nBLOQUES ASIGNADOS (del mas reciente al mas antiguo):\n";
        cout << "----------------------------------------------------\n";
        cout << "Posicion  ID Proc  Nombre Proceso          Tamanio\n";
        cout << "----------------------------------------------------\n";
        
        BloqueMemoria* actual = tope;
        int posicion = 1;
        
        // Recorrer toda la pila mostrando cada bloque
        while (actual != NULL) {
            // Formatear salida para alinear columnas
            cout << posicion;
            if (posicion < 10) cout << "         ";
            else cout << "        ";
            
            cout << actual->idProceso;
            if (actual->idProceso < 10) cout << "        ";
            else if (actual->idProceso < 100) cout << "       ";
            else cout << "      ";
            
            cout << actual->nombreProceso;
            int len = strlen(actual->nombreProceso);
            for (int i = len; i < 24; i++) cout << " ";
            
            cout << actual->tamanioMB << " MB\n";
            
            actual = actual->siguiente;
            posicion++;
        }
        
        cout << "====================================================\n";
    }
    
    // ============================================
    // MENÚ PRINCIPAL DEL GESTOR DE MEMORIA
    // Interfaz de usuario para interactuar con el sistema
    // ============================================
    void menuMemoria(GestorProcesos& gestor) {
        int opcion;
        
        do {
            cout << "\n======= GESTOR DE MEMORIA =======\n";
            cout << "1. Asignar memoria a proceso (PUSH)\n";
            cout << "2. Liberar ultimo bloque (POP)\n";
            cout << "3. Ver estado de la memoria\n";
            cout << "4. Volver al menu principal\n";
            cout << "=================================\n";
            cout << "Opcion: ";
            cin >> opcion;
            
            // Validar entrada del usuario
            if (cin.fail()) {
                cin.clear();
                cin.ignore(1000, '\n');
                cout << "Error: Ingrese un numero valido.\n";
                continue;
            }
            cin.ignore();
            
            switch(opcion) {
                case 1: { // ASIGNAR MEMORIA (PUSH)
                    // Verificar si hay procesos disponibles
                    if (gestor.estaVacia()) {
                        cout << "\n[ERROR] No hay procesos creados en el Gestor de Procesos.\n";
                        cout << "Debe crear procesos primero antes de asignar memoria.\n";
                        break;
                    }
                    
                    // Mostrar procesos disponibles
                    gestor.mostrar();
                    
                    int idSeleccionado;
                    int tamanioMB;
                    bool entradaValida = false;
                    
                    // Solicitar ID del proceso con validación
                    while (!entradaValida) {
                        cout << "\nIngrese el ID del proceso: ";
                        cin >> idSeleccionado;
                        
                        if (cin.fail()) {
                            cin.clear();
                            cin.ignore(1000, '\n');
                            cout << "Error: Debe ingresar un numero valido.\n";
                        } else if (idSeleccionado <= 0) {
                            cin.ignore(1000, '\n');
                            cout << "Error: El ID debe ser un numero positivo.\n";
                        } else {
                            // Buscar el proceso por ID
                            Proceso* p = gestor.buscar(idSeleccionado);
                            if (p == NULL) {
                                cin.ignore(1000, '\n');
                                cout << "Error: No existe ningun proceso con ese ID.\n";
                            } else {
                                entradaValida = true;
                                cin.ignore(1000, '\n');
                                
                                // Solicitar tamaño de memoria con validación
                                bool tamanioValido = false;
                                while (!tamanioValido) {
                                    cout << "Ingrese el tamanio de memoria en MB (1-1024): ";
                                    cin >> tamanioMB;
                                    
                                    if (cin.fail()) {
                                        cin.clear();
                                        cin.ignore(1000, '\n');
                                        cout << "Error: Debe ingresar un numero valido.\n";
                                    } else if (tamanioMB < 1 || tamanioMB > 1024) {
                                        cin.ignore(1000, '\n');
                                        cout << "Error: El tamanio debe estar entre 1 y 1024 MB.\n";
                                    } else {
                                        tamanioValido = true;
                                        cin.ignore(1000, '\n');
                                        
                                        // Asignar memoria al proceso
                                        asignarMemoria(p->id, p->nombre, tamanioMB);
                                    }
                                }
                            }
                        }
                    }
                    break;
                }
                
                case 2: // LIBERAR MEMORIA (POP)
                    liberarMemoria();
                    break;
                
                case 3: // VER ESTADO DE LA MEMORIA
                    mostrarEstadoMemoria();
                    break;
                
                case 4: // VOLVER AL MENÚ PRINCIPAL
                    cout << "Volviendo al menu principal...\n";
                    break;
                
                default:
                    cout << "Opcion invalida.\n";
            }
        } while(opcion != 4);
    }
    
    // ============================================
    // DESTRUCTOR DEL GESTOR DE MEMORIA
    // Libera toda la memoria asignada al destruir el objeto
    // ============================================
    ~GestorMemoria() {
        BloqueMemoria* actual = tope;
        while (actual != NULL) {
            BloqueMemoria* temp = actual;
            actual = actual->siguiente;
            delete temp;  // Liberar cada bloque de memoria
        }
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
        frente = NULL; 			// Inicializa la cola vac�a
        fin = NULL;
        ejecutados = NULL;		// Inicializa la lista de ejecutados vac�a
        cout << "[INFO] Planificador de CPU inicializado correctamente\n";
    }

    // Inserta un proceso en la cola seg�n su prioridad
    void encolar(Proceso* proc) {
    Nodo* nuevo = new Nodo(proc);		// Crear nuevo nodo para el proceso

    // Si la cola est� vac�a o tiene mayor prioridad
    if (frente == NULL || proc->prioridad > frente->proceso->prioridad) {
        nuevo->siguiente = frente;		// Nuevo nodo apunta al frente actual
        frente = nuevo;					// Nuevo nodo se convierte en el frente
    } else {	
    	// Buscar la posici�n correcta seg�n prioridad
        Nodo* temp = frente;
        while (temp->siguiente != NULL && temp->siguiente->proceso->prioridad >= proc->prioridad) {
            temp = temp->siguiente;
        }
        nuevo->siguiente = temp->siguiente;	// Insertar nodo en la posici�n
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

        // Ajuste de espacios seg�n tama�o del ID
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

        // Validaci�n  de entrada
        if (cin.fail()) {
            cin.clear();                // Limpiar error de entrada
            cin.ignore(1000, '\n');     
            cout << "\nIngrese solo numeros validos.\n";
        } else {
            // Seleccionar acci�n seg�n opci�n ingresada
            switch (opcion) {

                case 1: { // Eliminar proceso por ID
                    if (ejecutados == NULL) {
                        // Si esta vac�a
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

                            // Si no se encontr� el ID
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
                        ejecutados = NULL; // Lista queda vac�a
                        cout << "\nTodos los procesos ejecutados han sido eliminados.\n";
                    }
                    break; 
                }

                case 3: // Salir del men�
                    cout << "\nRegresando al menu del planificador...\n";
                    break;

                default: // Opci�n inv�lida
                    cout << "\nOpcion no valida.\n";
                    break;
            }
        }

// Repetir mientras la opci�n no sea 3
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

        // Espacios seg�n tama�o del ID
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
    // Men� del planificador
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
    			while (!idValido) {		// Bucle para asegurar que se ingrese un ID v�lido
        			cout << "\nIngrese el ID del proceso que desea encolar: ";
        			cin >> idSeleccionado;

        		if (cin.fail()) {		// Si la entrada no es un n�mero
            		cin.clear();
            		cin.ignore(1000, '\n');
            		cout << "Ingrese un numero valido (solo digitos).\n";
        		}
        		else if (idSeleccionado <= 0) {  // Si el n�mero no es positivo
            		cout << "El ID debe ser un numero positivo.\n";
        		}
        		else {
            		p = gestor.buscar(idSeleccionado);		// Buscar proceso por ID
            		if (p == NULL) {
                		cin.ignore(1000, '\n');
                		cout << "No existe ningun proceso con ese ID.\n";
            	}
            	else {  // Si se encontr� el proceso
                	cin.ignore(1000, '\n'); 
                	idValido = true;       // ID v�lido, salir del bucle 
            }
        }
    }   			
    			if (p != NULL) {
        			Nodo* temp = frente;	// Nodo temporal para recorrer la cola
        			bool yaEncolado = false;	// Detectar si ya est� encolado

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
                case 6:		// Volver al men� principal
                cout << "Volviendo al menu principal...\n";
                	break;
                default:
                    cout << "Opcion invalida.\n";
            }
        } while (opcion != 6);
    }
};

// ============================================
// MEN� DEL GESTOR DE PROCESOS
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
        cout << "2. Gestor de Memoria [FUNCIONAL]\n";
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
                memoria.menuMemoria(gestor);
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

