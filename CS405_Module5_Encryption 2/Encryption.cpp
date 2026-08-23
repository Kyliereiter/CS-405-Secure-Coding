// Encryption.cpp : This file contains the 'main' function.
// Program execution begins and ends there.
//

#include <cassert>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

/// <summary>
/// Encrypt or decrypt a source string using the provided key.
/// XOR encryption uses the same function for encryption and decryption.
/// </summary>
/// <param name="source">Input string to process</param>
/// <param name="key">Key used for encryption or decryption</param>
/// <returns>Transformed string</returns>
std::string encrypt_decrypt(
    const std::string& source,
    const std::string& key)
{
    // Store the lengths so they do not need to be recalculated.
    const auto key_length = key.length();
    const auto source_length = source.length();

    // Confirm that the source and key contain data.
    assert(key_length > 0);
    assert(source_length > 0);

    // Start with a copy of the source string.
    std::string output = source;

    // Process each source character individually.
    for (size_t i = 0; i < source_length; ++i)
    {
        /*
         * XOR the source character with a character from the key.
         *
         * The modulo operator makes the program restart at the
         * beginning of the key when it reaches the end.
         *
         * Example:
         * key[i % key_length]
         *
         * If the key contains 8 characters, character 8 of the source
         * uses character 0 of the key again.
         */
        output[i] = source[i] ^ key[i % key_length];
    }

    // The transformed string must remain the same length.
    assert(output.length() == source_length);

    return output;
}

/// <summary>
/// Loads the complete contents of a text file into a string.
/// </summary>
/// <param name="filename">Name of the file to read</param>
/// <returns>Contents of the file</returns>
std::string read_file(const std::string& filename)
{
    /*
     * Open the file in binary mode.
     *
     * Binary mode helps preserve every character exactly as it appears,
     * including line breaks and any special characters.
     */
    std::ifstream input_file(filename, std::ios::binary);

    // Make sure the file opened successfully.
    if (!input_file.is_open())
    {
        std::cerr << "Error: Could not open input file: "
                  << filename << std::endl;

        return "";
    }

    // Copy the complete file buffer into a string stream.
    std::ostringstream file_stream;
    file_stream << input_file.rdbuf();

    // Check whether an error occurred while reading.
    if (input_file.bad())
    {
        std::cerr << "Error: A problem occurred while reading: "
                  << filename << std::endl;

        return "";
    }

    input_file.close();

    return file_stream.str();
}

/// <summary>
/// Gets the student's name from the first line of the input file.
/// </summary>
/// <param name="string_data">Complete contents of the input file</param>
/// <returns>Student name from the first line</returns>
std::string get_student_name(const std::string& string_data)
{
    std::string student_name;

    // Find the first newline character.
    size_t pos = string_data.find('\n');

    if (pos != std::string::npos)
    {
        // Copy everything before the first newline.
        student_name = string_data.substr(0, pos);

        /*
         * Remove a carriage return if the file uses Windows-style
         * line endings, which use \r\n instead of only \n.
         */
        if (!student_name.empty() && student_name.back() == '\r')
        {
            student_name.pop_back();
        }
    }
    else
    {
        // If there is no newline, use the complete string as the name.
        student_name = string_data;
    }

    return student_name;
}

/// <summary>
/// Saves the student name, date, key, and transformed data to a file.
/// </summary>
/// <param name="filename">Name of the output file</param>
/// <param name="student_name">Student name</param>
/// <param name="key">Encryption key</param>
/// <param name="data">Encrypted or decrypted data</param>
void save_data_file(
    const std::string& filename,
    const std::string& student_name,
    const std::string& key,
    const std::string& data)
{
    // Open the output file in binary mode.
    std::ofstream output_file(
        filename,
        std::ios::binary | std::ios::trunc);

    // Make sure the output file opened successfully.
    if (!output_file.is_open())
    {
        std::cerr << "Error: Could not create output file: "
                  << filename << std::endl;

        return;
    }

    // Get the current date and time.
    std::time_t current_time = std::time(nullptr);
    std::tm local_time{};

#ifdef _WIN32
    // Microsoft Visual Studio and Windows.
    localtime_s(&local_time, &current_time);
#else
    // macOS and Linux.
    localtime_r(&current_time, &local_time);
#endif

    /*
     * Required file format:
     *
     * Line 1: Student name
     * Line 2: Date in yyyy-mm-dd format
     * Line 3: Encryption key
     * Line 4 and after: Data
     */
    output_file << student_name << '\n';

    output_file << std::put_time(
        &local_time,
        "%Y-%m-%d") << '\n';

    output_file << key << '\n';

    /*
     * Use write() instead of the insertion operator because encrypted
     * data can contain spaces, null bytes, or other nonprinting characters.
     */
    output_file.write(
        data.data(),
        static_cast<std::streamsize>(data.size()));

    // Confirm that writing was successful.
    if (!output_file)
    {
        std::cerr << "Error: A problem occurred while writing: "
                  << filename << std::endl;
    }

    output_file.close();
}

int main()
{
    std::cout << "Encryption Decryption Test!" << std::endl;

    /*
     * Input file format:
     *
     * Line 1: Student name
     * Line 2: Lorem Ipsum Generator website used
     * Lines 3+: Three paragraphs of generated text
     */

    const std::string file_name = "inputdatafile.txt";
    const std::string encrypted_file_name = "encrypteddatafile.txt";

    /*
     * Keep this spelling because it is the filename defined
     * in the original starter code.
     */
    const std::string decrypted_file_name =
        "decrytpteddatafile.txt";

    const std::string key = "password";

    // Load the complete input file.
    const std::string source_string = read_file(file_name);

    // Stop the program if the input file was empty or could not be read.
    if (source_string.empty())
    {
        std::cerr << "Error: The input file is empty or unavailable."
                  << std::endl;

        return 1;
    }

    // Get the student's name from the first line.
    const std::string student_name =
        get_student_name(source_string);

    // Encrypt the original file contents.
    const std::string encrypted_string =
        encrypt_decrypt(source_string, key);

    // Save the encrypted data.
    save_data_file(
        encrypted_file_name,
        student_name,
        key,
        encrypted_string);

    // Decrypt the encrypted string using the same key.
    const std::string decrypted_string =
        encrypt_decrypt(encrypted_string, key);

    // Save the decrypted data.
    save_data_file(
        decrypted_file_name,
        student_name,
        key,
        decrypted_string);

    std::cout
        << "Read File: " << file_name
        << " - Encrypted To: " << encrypted_file_name
        << " - Decrypted To: " << decrypted_file_name
        << std::endl;

    // Verify that decryption restored the original data.
    if (decrypted_string == source_string)
    {
        std::cout
            << "Verification successful: decrypted data matches "
            << "the original data."
            << std::endl;
    }
    else
    {
        std::cout
            << "Verification failed: decrypted data does not match "
            << "the original data."
            << std::endl;
    }

    return 0;
}