#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

class Phase1
{
    char M[100][4];  // Memory
    char IR[4];      // Instruction Register
    char R[4];       // General-purpose Register
    char buffer[40]; // Buffer for reading input
    int IC;          // Instruction Counter
    int SI;          // System Indicator
    bool C;          // Condition flag
    ifstream Input;
    ofstream Output;

public:
    // Initialize the OS
    void INIT()
    {
        // Clear memory and registers
        for (int i = 0; i < 100; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                M[i][j] = ' '; // Empty space instead of \0
            }
        }
        for (int i = 0; i < 4; i++)
        {
            IR[i] = ' ';
            R[i] = ' ';
        }
        C = false;
        IC = 0;
        SI = 0;
    }

    // Load program into memory
    void LOAD()
    {
        string line;
        while (getline(Input, line))
        {
            // $AMJ - start of program
            if (line.substr(0, 4) == "$AMJ")
            {
                INIT();
            }
            // $DTA - start of data
            else if (line.substr(0, 4) == "$DTA")
            {
                STARTEXECUTION();
            }
            // $END - end of program
            else if (line.substr(0, 4) == "$END")
            {
                break;
            }
            // Load instructions/data into memory
            else
            {
                int memIndex = IC; // Store at current IC position
                int bufferIndex = 0;
                while (bufferIndex < line.length() && memIndex < 100)
                {
                    for (int j = 0; j < 4; j++)
                    {
                        if (bufferIndex < line.length())
                        {
                            M[memIndex][j] = line[bufferIndex++];
                        }
                        else
                        {
                            M[memIndex][j] = ' '; // Padding with space
                        }
                    }
                    memIndex++;
                }
                IC = memIndex; // Update IC
            }
        }
    }

    // Start program execution
    void STARTEXECUTION()
    {
        IC = 0;
        try
        {
            EXECUTEUSERPROGRAM();
        }
        catch (const exception &e)
        {
            cerr << e.what() << endl;
        }
    }

    // Execute the user program
    void EXECUTEUSERPROGRAM()
    {
        while (true)
        {
            // Fetch instruction
            for (int i = 0; i < 4; i++)
            {
                IR[i] = M[IC][i];
            }
            IC++;

            int address = (IR[2] - '0') * 10 + (IR[3] - '0'); // Convert last two characters to int address

            switch (IR[0])
            {
            case 'G':
                if (IR[1] == 'D')
                { // Get data
                    SI = 1;
                    MOS();
                }
                break;
            case 'P':
                if (IR[1] == 'D')
                { // Put data
                    SI = 2;
                    MOS();
                }
                break;
            case 'H': // Halt
                SI = 3;
                MOS();
                return;
            case 'L':
                if (IR[1] == 'R')
                { // Load register
                    memcpy(R, M[address], 4);
                }
                break;
            case 'S':
                if (IR[1] == 'R')
                { // Store register
                    memcpy(M[address], R, 4);
                }
                break;
            case 'C':
                if (IR[1] == 'R')
                { // Compare
                    C = true;
                    for (int j = 0; j < 4; j++)
                    {
                        if (M[address][j] != R[j])
                        {
                            C = false;
                            break;
                        }
                    }
                }
                break;
            case 'B':
                if (IR[1] == 'T' && C)
                { // Branch on true
                    IC = address;
                }
                break;
            default:
                cout << "Invalid Instruction: " << string(IR, 4) << endl;
            }
        }
    }

    // Main OS Service Routine (MOS)
    void MOS()
    {
        switch (SI)
        {
        case 1:
            READ();
            break;
        case 2:
            WRITE();
            break;
        case 3:
            TERMINATE();
            break;
        }
    }

    // Read data into memory
    void READ()
    {
        string line;
        if (!getline(Input, line))
            return;

        // Reset buffer
        memset(buffer, ' ', sizeof(buffer));

        // Convert the string to char array
        strncpy(buffer, line.c_str(), line.length());

        int bufferIndex = 0;
        int memAddress = (IR[2] - '0') * 10; // Calculate starting memory address

        for (int i = 0; i < 10 && memAddress < 100; i++)
        { // Limit to 10 rows of memory
            for (int j = 0; j < 4; j++)
            {
                if (bufferIndex < line.length())
                {
                    M[memAddress][j] = buffer[bufferIndex++];
                }
                else
                {
                    M[memAddress][j] = ' '; // Padding with space
                }
            }
            memAddress++;
        }
        SI = 0; // Reset System Indicator
    }

    // Write data from memory to output
    void WRITE()
    {
        int memAddress = (IR[2] - '0') * 10; // Calculate starting memory address

        for (int i = 0; i < 10 && memAddress < 100; i++)
        { // Write 10 rows of memory
            for (int j = 0; j < 4; j++)
            {
                Output << M[memAddress][j];
            }
            Output << "\n";
            memAddress++;
        }
        SI = 0; // Reset System Indicator
    }

    // Terminate program
    void TERMINATE()
    {
        Output << "Program Terminated\n";
        SI = 0;
    }

    void run(const string &inputFilePath, const string &outputFilePath)
    {
        Input.open(inputFilePath);
        Output.open(outputFilePath);
        if (!Input || !Output)
        {
            cerr << "Error opening files!" << endl;
            return;
        }

        LOAD();
        Input.close();
        Output.close();
    }
};

int main()
{
    Phase1 os;

    os.run("C:\\C_DS_Class\\.vscode\\OS\\phase1\\input.txt",
           "C:\\C_DS_Class\\.vscode\\OS\\phase1\\output.txt");

    // os.run("C:/C_DS_Class/.vscode/OS/pahse1/input.txt",
    //        "C:/C_DS_Class/.vscode/OS/pahse1/output.txt");

    return 0;
}
