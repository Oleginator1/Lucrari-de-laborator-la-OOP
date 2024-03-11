#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <ctime>
#include <unordered_map>
#include <filesystem>
#include <chrono>
#include <thread>
#include <regex>


using namespace std;
namespace fs = filesystem;


class File
{
protected:
    string name;
    string extension;
    string path;
    time_t creation_time;
    time_t last_updated_time;

    



public:

    File(const string nm, const string ext, const string pth)
        :name(nm), extension(ext), path(pth), creation_time(get_time()) {}

    virtual ~File() = default;


    static time_t get_time()
    {
        return time(nullptr);
    }

    time_t get_last_updated_time() const
    {
        return last_updated_time;
    }

    time_t update_last_updated_time()
    {
        last_updated_time = get_time();
    }

    string get_Name() const
    {
        return name;
    }

    virtual void print_info() const = 0;


    string read_content()
    {
        ifstream file(path);
        ostringstream content;
        content << file.rdbuf();
        return content.str();
    }

    void update_file(const string& new_content)
    {
        ofstream file(path);
        file << new_content;
        update_last_updated_time();
    }

    string get_extension() const
    {
        return extension;

    }



};

class Text_File : public File
{
private:

    int line_count = 0, word_count = 0;
    size_t char_count = 0;
    unordered_map<string, int> previous_word_count;

    static string get_time_as_string(time_t time1)
    {
        char temp[80];
        strftime(temp, sizeof(temp), "%Y-%m-%dT-%H:%M:%S", localtime(&time1));
        return temp;
    }

    int count_words(const string line)
    {
        bool isword = false;
        int counter = 0;

        for (char c : line)
        {
            if (isalpha(c))
            {
                if (!isword)
                {
                    isword = true;
                    counter++;
                }

            }
            else
            {
                isword = false;
            }
        }
        return counter;
    }

public:

    Text_File(const string nm, const string ext, const string pth)
        :File(nm, ext, pth), line_count(0), word_count(0), char_count(0) {}




    void print_info() const override
    {
        cout << "Text Name: " << name << "." << extension << "\n";
        cout << "Path: " << path << "\n";
        cout << "Time Of Creation: " << get_time_as_string(creation_time) << "\n";
        cout << "Last Updated: " << get_time_as_string(last_updated_time) << "\n";
        cout << "Line Count: " << line_count << "\n";
        cout << "Word Count:" << word_count << "\n";
        cout << "Character Count: " << char_count << "\n";

    }

    void calculate_word_line_char()
    {
        ifstream file(path);
        string line;

        while (getline(file, line))
        {
            line_count++;
            char_count = line.size();
            word_count = count_words(line);
        }


    }




    void detect_change()
    {
        string current_content = read_content();
        istringstream current_stream(current_content);
        unordered_map<string, int> current_word_count;

        string word;
        while (current_stream >> word)
        {
            current_word_count[word]++;
        }

        for (const auto& temp : previous_word_count)
        {
            string word1 = temp.first;
            int count1 = temp.second;
            if (current_word_count[word1] != count1)
            {
                cout << "Word: " << word1;

                if (current_word_count[word1] > count1)
                {
                    cout << " added and has " << current_word_count[word1] - count1 << " occurances\n";
                }
                else
                {
                    cout << " deleted and has " << count1 - current_word_count[word1] << " occurances\n";
                }
            }

        }
        previous_word_count = move(current_word_count);


    }



};



class Image_File : public File
{
private:
    string image_size;

    static string get_time_as_string(time_t time1)
    {
        char temp[80];
        strftime(temp, sizeof(temp), "%Y-%m-%dT-%H:%M:%S", localtime(&time1));
        return temp;
    }


public:

    Image_File(const string nm, const string ext, const string pth)
        :File(nm, ext, pth), image_size("0x0") {}



    void print_info() const override
    {
        cout << "Image Name: " << name << "." << extension << "\n";
        cout << "Path: " << path << "\n";
        cout << "Time Of Creation: " << get_time_as_string(creation_time) << "\n";
        cout << "Last Updated: " << get_time_as_string(last_updated_time) << "\n";
        cout << "Image Size: " << image_size << "\n";
    }

    void calculate_image_size()
    {
        image_size = "1024x860";
        update_last_updated_time();
    }


};

class Change_Detector
{

private:
    string folder_location = "C:/Oleg_School/Lucrari de laborator C&C++/Asistenta pentru programarea orientata pe obiecte/Lab3";
    vector<string> last_snapshot_files;

    time_t snapshot_time;
    vector<File*> files;

    thread detection_thread;




    static string get_time_as_string(time_t time1)
    {
        char temp[80];
        strftime(temp, sizeof(temp), "%Y-%m-%dT-%H:%M:%S", localtime(&time1));
        return temp;
    }

public:

    Change_Detector(const string& folder_path);

    void commit()
    {
        snapshot_time = File::get_time();
        cout << "Snapshot created at: " << get_time_as_string(snapshot_time);

    }

    void file_info(const string& file_name)
    {
        for (const auto& file : files)
        {
            if (file->get_Name() == file_name)
            {
                file->print_info();
                return;
            }


        }
        cout << "File " << file_name << "not found";
    }


    void status()
    {
        cout << "Status:\n";
        for (const auto& file : files)
        {
            if (file->get_last_updated_time() > snapshot_time)
            {
                cout << file->get_Name() << "." << file->get_extension() << " - Changed\n";
            }
            else if (find(last_snapshot_files.begin(), last_snapshot_files.end(), file->get_Name()) == last_snapshot_files.end())
            {
                cout << file->get_Name() << "." << file->get_extension() << " - New file" << endl;
            }
            else
            {
                cout << file->get_Name() << "." << file->get_extension() << " - No Change\n";
            }
        }
        cout << "> ";

    }


    

    

    void start_detection();
    void stop_detection();

    void detect_file_changes(bool scheduled = false);

};

Change_Detector::Change_Detector(const string& folder_path)
    : folder_location(folder_path)
{
    
    snapshot_time = File::get_time();

    for (const auto& entry : fs::directory_iterator(folder_location))
    {
        if (entry.is_regular_file())
        {
            last_snapshot_files.push_back(entry.path().filename().string());
        }
    }
}

void Change_Detector::start_detection()
{
    detection_thread = thread([this]
        {
            while (true)
            {
                this_thread::sleep_for(chrono::seconds(5));
                detect_file_changes(true);
            }
        });
}


void Change_Detector::stop_detection()
{
    if (detection_thread.joinable())
    {
        detection_thread.join();
    }
}

void Change_Detector::detect_file_changes(bool scheduled)
{
    vector<string> current_files;

    for (const auto& entry : fs::directory_iterator(folder_location))
    {
        if (entry.is_regular_file())
        {
            current_files.push_back(entry.path().filename().string());
        }
    }

    for (const auto& new_file : current_files)
    {
        if (find(last_snapshot_files.begin(), last_snapshot_files.end(), new_file) == last_snapshot_files.end())
        {
            if (scheduled)
            {
                cout << "Scheduled Detection: " << new_file << " - New File" << endl;

            }
            else
            {
                cout << new_file << " - New file" << endl;
            }

        }
    }

    for (const auto& deleted_file : last_snapshot_files)
    {
        if (find(current_files.begin(), current_files.end(), deleted_file) == current_files.end())
        {
            if (scheduled)
            {
                cout << "Scheduled Detection: " << deleted_file << " - Deleted" << endl;

            }
            else
            {
                cout << deleted_file << " - Deleted file" << endl;
            }
        }

    }

    last_snapshot_files = move(current_files);

}



int main()
{
    Change_Detector detector("C:/Oleg_School/Lucrari de laborator C&C++/Asistenta pentru programarea orientata pe obiecte/Lab3");
    
    string input;

    detector.stop_detection();

    while (true)
    {
        cout << "***Menu*** " << endl << endl;
        cout << "Enter command:" << endl;
        cout << "commit" << endl;
        cout << "information <filename> " << endl;
        cout << "status" << endl;
        //cout << "modify <filename>" << endl;
        getline(cin, input);

        istringstream iss(input);

        vector<string> tokens(istream_iterator<string>{iss}, istream_iterator<string>());

        if (tokens.empty())
        {
            cout << "Wrong input try again\n";
            break;
        }

        if (tokens[0] == "commit")
        {
            detector.commit();
        }
        else if (tokens[0] == "information" && tokens.size() == 2)
        {
            detector.file_info(tokens[1]);
        }
        else if (tokens[0] == "status")
        {
            detector.status();

        }
        



    }
    return 0;
}
