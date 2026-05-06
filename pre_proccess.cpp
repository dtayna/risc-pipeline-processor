#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <algorithm>
#include <iomanip>
#include <stdexcept>

static int parse_reg(const std::string& token) {
    if (token.size() < 2 || token[0] != 'R') {
        throw std::runtime_error("Invalid register: " + token);
    }
    int r = std::stoi(token.substr(1));
    if (r < 0 || r > 15) {
        throw std::runtime_error("Register out of range R0..R15: " + token);
    }
    return r;
}

static int parse_number(const std::string& token) {
    return std::stoi(token, nullptr, 0);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file.asm> <output_file.bin>" << std::endl;
        return 1;
    }

    std::ifstream input(argv[1]);
    std::ofstream output(argv[2]);

    if (!input.is_open() || !output.is_open()) {
        std::cerr << "Error opening files!" << std::endl;
        return 1;
    }

    std::map<std::string, int> opcodes = {
        {"AND",  0x0}, {"OR",   0x1}, {"XOR",  0x2}, {"NOT",  0x3},
        {"CMP",  0x4}, {"ADD",  0x5}, {"SUB",  0x6}, {"ADDI", 0x7},
        {"LD",   0xA}, {"ST",   0xB}, {"J",    0xC}, {"JZ",   0xD},
        {"JN",   0xE}, {"NOP",  0xF}
    };

    std::string line;
    int line_count = 0;

    while (std::getline(input, line)) {
        size_t comment_pos = line.find(';');
        if (comment_pos != std::string::npos) line = line.substr(0, comment_pos);

        std::replace(line.begin(), line.end(), ',', ' ');
        std::transform(line.begin(), line.end(), line.begin(), ::toupper);

        std::stringstream ss(line);
        std::string mnemonic;
        if (!(ss >> mnemonic)) continue;

        if (!opcodes.count(mnemonic)) {
            std::cerr << "Unknown instruction ignored: " << mnemonic << std::endl;
            continue;
        }

        try {
            int op = opcodes[mnemonic];
            unsigned short instruction = 0;
            std::string t1, t2, t3;

            if (op == 0xF) {
                instruction = 0xF000;
            }
            else if (op == 0xC || op == 0xD || op == 0xE) {
                int address = 0;
                if (ss >> t1) {
                    if (t1[0] == 'R') {
                        if (ss >> t2) address = parse_number(t2);
                    } else {
                        address = parse_number(t1);
                    }
                }
                instruction = static_cast<unsigned short>((op << 12) | (address & 0x0FFF));
            }
            else if (op == 0x7) {
                int rd = 0, imm = 0;
                if (ss >> t1) rd = parse_reg(t1);

                if (ss >> t2) {
                    if (t2[0] == 'R') {
                        if (ss >> t3) imm = parse_number(t3);
                    } else {
                        imm = parse_number(t2);
                    }
                }

                if (imm < -128 || imm > 255) {
                    throw std::runtime_error("ADDI immediate out of 8-bit range: " + std::to_string(imm));
                }

                instruction = static_cast<unsigned short>((op << 12) | (rd << 8) | (imm & 0xFF));
            }
            else if (op == 0xA) {
                int rd = 0, addr = 0;
                if (ss >> t1) rd = parse_reg(t1);

                if (ss >> t2) {
                    if (t2[0] == 'R') {
                        int base = parse_reg(t2);
                        int off = 0;
                        if (ss >> t3) off = parse_number(t3);

                        if (off < 0 || off > 15) {
                            throw std::runtime_error("LD offset out of 4-bit range (0..15): " + std::to_string(off));
                        }

                        addr = ((base & 0xF) << 4) | (off & 0xF);
                    } else {
                        addr = parse_number(t2);
                    }
                }

                if (addr < 0 || addr > 255) {
                    throw std::runtime_error("LD address out of 8-bit range (0..255): " + std::to_string(addr));
                }

                instruction = static_cast<unsigned short>((op << 12) | (rd << 8) | (addr & 0xFF));
            }
            else if (op == 0xB) {
                int rsrc = 0, addr = 0;
                if (ss >> t1) rsrc = parse_reg(t1);

                if (ss >> t2) {
                    if (t2[0] == 'R') {
                        int base = parse_reg(t2);
                        int off = 0;
                        if (ss >> t3) off = parse_number(t3);

                        if (off < 0 || off > 15) {
                            throw std::runtime_error("ST offset out of 4-bit range (0..15): " + std::to_string(off));
                        }

                        addr = ((base & 0xF) << 4) | (off & 0xF);
                    } else {
                        addr = parse_number(t2);
                    }
                }

                if (addr < 0 || addr > 255) {
                    throw std::runtime_error("ST address out of 8-bit range (0..255): " + std::to_string(addr));
                }

                instruction = static_cast<unsigned short>((op << 12) | (rsrc << 8) | (addr & 0xFF));
            }
            else {
                int rd = 0, rs1 = 0, rs2 = 0;
                if (ss >> t1) rd = parse_reg(t1);
                if (ss >> t2) rs1 = parse_reg(t2);
                if (ss >> t3) rs2 = parse_reg(t3);
                instruction = static_cast<unsigned short>((op << 12) | (rd << 8) | (rs1 << 4) | (rs2 & 0xF));
            }

            output << std::setfill('0') << std::setw(4) << std::hex << instruction << std::endl;
            line_count++;
        }
        catch (const std::exception& e) {
            std::cerr << "Error on line: '" << line << "' -> " << e.what() << std::endl;
            return 1;
        }
    }

    std::cout << "Success! " << line_count << " instructions translated." << std::endl;
    return 0;
}