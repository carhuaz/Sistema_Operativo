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
        int id;					// ID único del proceso
        string nombre;
        int prioridad;			// Nivel de prioridad (1-10)
        Nodo* siguiente;		// Apunta al siguiente proceso
	// Constructor del nodo
        Nodo(int _id, string _nombre, int _prioridad) {
            id = _id;
            nombre = _nombre;
            prioridad = _prioridad;
            siguiente = NULL;
        }
    };

    Nodo* frente;       // Cola principal
    Nodo* ejecutados;   // Lista de procesos ejecutados
    int contadorID;		

public:
	// Constructor 
    PlanificadorCPU() {
        frente = NULL;
        ejecutados = NULL;
        contadorID = 1;
        cout << "[INFO] Planificador de CPU inicializado correctamente\n";
    }

    // Inserta proceso en orden según su prioridad
    void encolar(string nombre, int prioridad) {
        Nodo* nuevo = new Nodo(contadorID++, nombre, prioridad);
		// Si la cola está vacía o el nuevo tiene prioridad mayor, se inserta al inicio
        if (frente == NULL || prioridad > frente->prioridad) {
            nuevo->siguiente = frente;
            frente = nuevo;
        } else {
            Nodo* temp = frente;
            // Avanza mientras la prioridad siguiente sea mayor o igual
            while (temp->siguiente != NULL && temp->siguiente->prioridad >= prioridad) {
                temp = temp->siguiente;
            }
            nuevo->siguiente = temp->siguiente;
            temp->siguiente = nuevo;
        }

        cout << "\nProceso '" << nombre << "' encolado con prioridad " << prioridad << ".\n ";
        cout << "\nProceso encolado correctamente en la cola de prioridad.\n ";
    }

    // Desencola (ejecuta) el proceso con mayor prioridad
    void ejecutar() {
        if (frente == NULL) {
            cout << "\n[!] No hay procesos en la cola.\n";
            return;
        }

        Nodo* temp = frente;		// Tomamos el proceso con mayor prioridad
        frente = frente->siguiente; // Lo retiramos de la cola

        cout << "\n>>> Ejecutando proceso: " << temp->nombre
             << " (Prioridad: " << temp->prioridad << ")\n";

        // Agregar a la lista de ejecutados
        temp->siguiente = ejecutados;
        ejecutados = temp;
    }

    // Muestra los procesos que ya fueron ejecutados
    void mostrarEjecutados() {
        if (ejecutados == NULL) {
            cout << "\n[!] Aun no hay procesos ejecutados.\n";
            return;
        }

        cout << "\n=== HISTORIAL DE PROCESOS EJECUTADOS ===\n";
        cout << "ID\tNombre\t\tPrioridad\n";
        cout << "---------------------------------------\n";
        Nodo* temp = ejecutados;
        while (temp != NULL) {
            cout << temp->id << "\t" << temp->nombre << "\t\t" << temp->prioridad << "\n";
            temp = temp->siguiente;
        }
        cout << "---------------------------------------\n";
    }

    // Muestra la cola actual
    void mostrarCola() {
        if (frente == NULL) {
            cout << "\n[!] No hay procesos en la cola actualmente.\n";
            return;
        }

        cout << "\n=== COLA DE PROCESOS (por prioridad: mayor -> menor) ===\n";
        cout << "ID\tNombre\t\tPrioridad\n";
        cout << "---------------------------------------\n";
        Nodo* temp = frente;
        while (temp != NULL) {
            cout << temp->id << "\t" << temp->nombre << "\t\t" << temp->prioridad << "\n";
            temp = temp->siguiente;
        }
        cout << "---------------------------------------\n";
    }

    // Menú del planificador
    void menuPlanificador() {
        int opcion;
        string nombre;
        int prioridad;

        do {
            cout << "\n======= PLANIFICADOR DE CPU =======\n";
            cout << "1. Encolar proceso\n";
            cout << "2. Ejecutar proceso (desencolar)\n";
            cout << "3. Mostrar cola actual\n";
            cout << "4. Mostrar procesos ejecutados\n";
            cout << "5. Volver al menu principal\n";
            cout << "===================================\n";
            cout << "Opcion: ";
            cin >> opcion;
            cin.ignore();

            switch (opcion) {
                case 1:
                    cout << "\nNombre del proceso: ";
                    getline(cin, nombre);
                    cout << "Prioridad (1-10): ";
                    cin >> prioridad;
                    cin.ignore();
                    encolar(nombre, prioridad);
                    break;
                case 2:
                    ejecutar();
                    break;
                case 3:
                    mostrarCola();
                    break;
                case 4:
                    mostrarEjecutados();
                    break;
                case 5:
                    cout << "Volviendo al menu principal...\n";
                    break;
                default:
                    cout << "Opcion invalida.\n";
            }
        } while (opcion != 5);
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
                planificador.menuPlanificador();
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

