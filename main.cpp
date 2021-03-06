#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

const int DAY_TITLE_MAX_LENGTH = 10;
const vector<string> DAYS = {"Saturday",  "Sunday",   "Monday", "Tuesday",
                             "Wednesday", "Thursday", "Friday"};
const int HALF_HOUR_COLUMNS = 5;
const int TOTAL_COLUMNS = 175;

vector<string> ReadHeader(string Line) {
    vector<string> Header;
    string ColName;
    stringstream SStream(Line);
    while (getline(SStream, ColName, ',')) {
        Header.push_back(ColName);
    }
    return Header;
}

map<string, string> ReadMovie(vector<string> Header, string Line) {
    string RecordValue;
    map<string, string> TempMovie;
    stringstream SStream(Line);
    for (auto ColName : Header) {
        getline(SStream, RecordValue, ',');
        TempMovie[ColName] = RecordValue;
    }
    return TempMovie;
}

vector<map<string, string>> ReadCSVFile(string FileName) {
    vector<map<string, string>> Movies;
    ifstream File;
    File.open(FileName);
    string Line;
    getline(File, Line);
    vector<string> Header = ReadHeader(Line);
    while (getline(File, Line)) Movies.push_back(ReadMovie(Header, Line));
    File.close();
    return Movies;
}

vector<string> GetMovieNames(const vector<map<string, string>>& Movies) {
    vector<string> MovieNames;
    for (auto Movie : Movies) MovieNames.push_back(Movie["MovieName"]);
    return MovieNames;
}

void PrintMovies(const vector<map<string, string>>& Movies) {
    vector<string> MovieNames = GetMovieNames(Movies);
    sort(MovieNames.begin(), MovieNames.end());
    // remove duplicated values
    MovieNames.erase(unique(MovieNames.begin(), MovieNames.end()),
                     MovieNames.end());
    for (auto MovieName : MovieNames) cout << MovieName << endl;
}

string RemoveCommand(string UserInput, const string& Command) {
    int index = 0;
    index = UserInput.find(Command, index);
    UserInput.replace(index, Command.length(), "");
    return UserInput;
}

string GetMovieNameFromInput(string UserInput) {
    return RemoveCommand(UserInput, "GET SCHEDULE ");
}

bool MovieExists(const vector<map<string, string>>& Movies, string MovieName) {
    for (auto Movie : Movies)
        if (Movie["MovieName"] == MovieName) return true;
    return false;
}

void PrintHours() {
    cout << "          08:00";
    cout << "               10:00";
    cout << "               12:00";
    cout << "               14:00";
    cout << "               16:00";
    cout << "               18:00";
    cout << "               20:00";
    cout << "               22:00";
    cout << "               00:00" << endl;
}

vector<map<string, string>> GetMovieScheduleOfDay(
    const vector<map<string, string>>& Movies, string MovieName, string Day) {
    vector<map<string, string>> Schedule;
    for (auto Movie : Movies) {
        if (Movie["MovieName"] == MovieName && Movie["Day"] == Day) {
            Schedule.push_back(Movie);
        }
    }
    return Schedule;
}

bool CompareCinemaNames(const map<string, string>& FirstMovie,
                        const map<string, string>& SecondMovie) {
    return FirstMovie.find("CinemaName")->second <
           SecondMovie.find("CinemaName")->second;
}

bool ComparePrices(const map<string, string>& FirstMovie,
                   const map<string, string>& SecondMovie) {
    return FirstMovie.find("Price")->second < SecondMovie.find("Price")->second;
}

bool CompareTimes(const map<string, string>& FirstMovie,
                  const map<string, string>& SecondMovie) {
    return FirstMovie.find("StartingTime")->second <
           SecondMovie.find("StartingTime")->second;
}

bool HaveInterference(const map<string, string>& FirstMovie,
                      const map<string, string>& SecondMovie) {
    if (FirstMovie.find("StartingTime")->second <
            SecondMovie.find("StartingTime")->second &&
        FirstMovie.find("FinishingTime")->second >
            SecondMovie.find("StartingTime")->second)
        return true;
    if (FirstMovie.find("StartingTime")->second ==
            SecondMovie.find("StartingTime")->second &&
        FirstMovie.find("FinishingTime")->second ==
            SecondMovie.find("FinishingTime")->second)
        return true;
    return false;
}

vector<map<string, string>> SortSchedule(vector<map<string, string>> Schedule) {
    // Sort with CinemaName
    sort(Schedule.begin(), Schedule.end(), CompareCinemaNames);
    // Sort with Price
    sort(Schedule.begin(), Schedule.end(), ComparePrices);
    // Sort with Starting Time
    sort(Schedule.begin(), Schedule.end(), CompareTimes);

    vector<map<string, string>> ChosenSchedule;
    ChosenSchedule.push_back(Schedule[0]);

    for (int i = 1; i < Schedule.size(); i++) {
        if (!HaveInterference(ChosenSchedule.back(), Schedule[i]))
            ChosenSchedule.push_back(Schedule[i]);
    }
    return ChosenSchedule;
}

float TimeToNumber(const string& Time) {
    if (Time == "00:00") return 12 * 2;
    stringstream SStream(Time);
    string Hour, Minute;
    getline(SStream, Hour, ':');
    getline(SStream, Minute, ':');
    float M = stoi(Minute);
    return stoi(Hour) * 2 + M / 30;
}

int StartingTimeToColumn(const string& Time) {
    return DAY_TITLE_MAX_LENGTH + 1 +
           (TimeToNumber(Time) - TimeToNumber("08:00")) * HALF_HOUR_COLUMNS;
}

int BlockLength(const string& StartingTime, const string& FinishingTime) {
    return (TimeToNumber(FinishingTime) - TimeToNumber(StartingTime)) * 5 - 1;
}

void PrintCharacters(int Length, char Character) {
    for (int i = 0; i < Length; i++) cout << Character;
}

vector<pair<int, int>> ConvertScheduleToColumns(
    vector<map<string, string>> Schedule) {
    sort(Schedule.begin(), Schedule.end(), CompareTimes);

    vector<pair<int, int>> Columns;
    for (auto Movie : Schedule) {
        Columns.push_back(
            make_pair(StartingTimeToColumn(Movie["StartingTime"]),
                      StartingTimeToColumn(Movie["FinishingTime"])));
    }
    return Columns;
}

bool CheckPositiveSign(int ColumnNo, const vector<pair<int, int>>& Columns) {
    for (auto Column : Columns)
        if (ColumnNo == Column.first || ColumnNo == Column.second) return true;
    return false;
}

bool CheckNegativeSign(int ColumnNo, const vector<pair<int, int>>& Columns) {
    for (auto Column : Columns)
        if (ColumnNo > Column.first && ColumnNo < Column.second) return true;
    return false;
}

void PrintBoxes(const vector<map<string, string>>& FirstSchedule,
                const vector<map<string, string>>& SecondSchedule) {
    vector<map<string, string>> Schedule = FirstSchedule;
    Schedule.insert(Schedule.end(), SecondSchedule.begin(),
                    SecondSchedule.end());
    vector<pair<int, int>> Columns = ConvertScheduleToColumns(Schedule);

    for (int ColumnNo = 1; ColumnNo < TOTAL_COLUMNS; ColumnNo++) {
        if (CheckPositiveSign(ColumnNo, Columns)) {
            cout << "+";
            continue;
        }
        if (CheckNegativeSign(ColumnNo, Columns)) {
            cout << "-";
            continue;
        }
        cout << " ";
    }
    cout << " " << endl;
}

void PrintDay(const string& Day, const vector<map<string, string>>& Schedule) {
    cout << Day;
    PrintCharacters(DAY_TITLE_MAX_LENGTH - Day.length(), ' ');
    int ColumnNo = DAY_TITLE_MAX_LENGTH + 1;
    for (auto Movie : Schedule) {
        int TileLength =
            BlockLength(Movie["StartingTime"], Movie["FinishingTime"]);
        int StartingColumn = StartingTimeToColumn(Movie["StartingTime"]);
        PrintCharacters(StartingColumn - ColumnNo, ' ');
        if (StartingColumn + 1 != ColumnNo) cout << "|";
        cout << Movie["CinemaName"];
        PrintCharacters(TileLength - Movie["CinemaName"].length(), ' ');
        cout << "|";
        ColumnNo = StartingColumn + TileLength + 2;
    }
    PrintCharacters(TOTAL_COLUMNS - ColumnNo + 1, ' ');
    cout << endl;
}

void PrintMovieSchedule(const vector<map<string, string>>& Movies,
                        string MovieName) {
    PrintHours();
    vector<map<string, string>> YesterdaySchedule =
        SortSchedule(GetMovieScheduleOfDay(Movies, MovieName, "Saturday"));
    vector<map<string, string>> TodaySchedule;
    for (auto Day : DAYS) {
        TodaySchedule =
            SortSchedule(GetMovieScheduleOfDay(Movies, MovieName, Day));
        PrintBoxes(TodaySchedule, YesterdaySchedule);
        PrintDay(Day, TodaySchedule);
        YesterdaySchedule = TodaySchedule;
    }
    PrintBoxes(YesterdaySchedule, YesterdaySchedule);
}

int CalculateWidth(const map<string, string>& Movie) {
    return (TimeToNumber(Movie.find("FinishingTime")->second) -
            TimeToNumber(Movie.find("StartingTime")->second)) *
           50;
}

int CalculateLeft(const map<string, string>& Movie) {
    return (TimeToNumber(Movie.find("StartingTime")->second) -
            TimeToNumber("08:00")) *
               50 +
           100;
}

int CalculateTop(string Day) {
    int DayNo;
    for (int i = 0; i < DAYS.size(); i++)
        if (Day == DAYS[i]) DayNo = i;

    return 60 + DayNo * 50;
}

void HTMLMovieSchedule(const vector<map<string, string>>& Movies,
                       string MovieName) {
    ofstream HTML(MovieName + ".html");
    HTML << "<html lang=\"en\">" << endl;
    HTML << "<head>" << endl;
    HTML << "<title></title>" << endl;
    HTML << "<link rel=\"stylesheet\" href=\"./style.css\" />" << endl;
    HTML << "</head>" << endl;
    HTML << "<body>" << endl;
    HTML << "<div class=\"time-box\" style=\"left: 100px;\"><p>08:00</p></div>"
         << endl;
    HTML << "<div class=\"time-box\" style=\"left: 300px;\"><p>10:00</p></div>"
         << endl;
    HTML << "<div class=\"time-box\" style=\"left: 500px;\"><p>12:00</p></div>"
         << endl;
    HTML << "<div class=\"time-box\" style=\"left: 700px;\"><p>14:00</p></div>"
         << endl;
    HTML << "<div class=\"time-box\" style=\"left: 900px;\"><p>16:00</p></div>"
         << endl;
    HTML << "<div class=\"time-box\" style=\"left: 1100px;\"><p>18:00</p></div>"
         << endl;
    HTML << "<div class=\"time-box\" style=\"left: 1300px;\"><p>20:00</p></div>"
         << endl;
    HTML << "<div class=\"time-box\" style=\"left: 1500px;\"><p>22:00</p></div>"
         << endl;
    HTML << "<div class=\"time-box\" style=\"left: 1700px;\"><p>00:00</p></div>"
         << endl;
    HTML << "<div class=\"vertical-line\" style=\"left: 100px;\"></div>"
         << endl;
    HTML << "<div class=\"vertical-line\" style=\"left: 200px;\"></div>"
         << endl;
    HTML << "<div class=\"vertical-line\" style=\"left: 300px;\"></div>"
         << endl;
    HTML << "<div class=\"vertical-line\" style=\"left: 400px;\"></div>"
         << endl;
    HTML << "<div class=\"vertical-line\" style=\"left: 500px;\"></div>"
         << endl;
    HTML << "<div class=\"vertical-line\" style=\"left: 600px;\"></div>"
         << endl;
    HTML << "<div class=\"vertical-line\" style=\"left: 700px;\"></div>"
         << endl;
    HTML << "<div class=\"vertical-line\" style=\"left: 800px;\"></div>"
         << endl;
    HTML << "<div class=\"vertical-line\" style=\"left: 900px;\"></div>"
         << endl;
    HTML << "<div class=\"vertical-line\" style=\"left: 1000px;\"></div>"
         << endl;
    HTML << "<div class=\"vertical-line\" style=\"left: 1100px;\"></div>"
         << endl;
    HTML << "<div class=\"vertical-line\" style=\"left: 1200px;\"></div>"
         << endl;
    HTML << "<div class=\"vertical-line\" style=\"left: 1300px;\"></div>"
         << endl;
    HTML << "<div class=\"vertical-line\" style=\"left: 1400px;\"></div>"
         << endl;
    HTML << "<div class=\"vertical-line\" style=\"left: 1500px;\"></div>"
         << endl;
    HTML << "<div class=\"vertical-line\" style=\"left: 1600px;\"></div>"
         << endl;
    HTML << "<div class=\"vertical-line\" style=\"left: 1700px;\"></div>"
         << endl;
    HTML << "<div class=\"day-box\" style=\"top: 60px;\">Saturday</div>"
         << endl;
    HTML << "<div class=\"day-box\" style=\"top: 110px;\">Sunday</div>" << endl;
    HTML << "<div class=\"day-box\" style=\"top: 160px;\">Monday</div>" << endl;
    HTML << "<div class=\"day-box\" style=\"top: 210px;\">Tuesday</div>"
         << endl;
    HTML << "<div class=\"day-box\" style=\"top: 260px;\">Wednesday</div>"
         << endl;
    HTML << "<div class=\"day-box\" style=\"top: 310px;\">Thursday</div>"
         << endl;
    HTML << "<div class=\"day-box\" style=\"top: 360px;\">Friday</div>" << endl;

    vector<map<string, string>> TodaySchedule;
    for (auto Day : DAYS) {
        TodaySchedule =
            SortSchedule(GetMovieScheduleOfDay(Movies, MovieName, Day));
        for (auto Movie : TodaySchedule) {
            HTML << "<div class=\"record-box\" style=\"width: "
                 << CalculateWidth(Movie)
                 << "px; left: " << CalculateLeft(Movie)
                 << "px; top:" << CalculateTop(Day) << "px; \">"
                 << Movie["CinemaName"] << "</div>";
        }
    }

    HTML << "</body>" << endl;
    HTML << "</html>" << endl;
    HTML.close();
}

void HandleUserInput(const vector<map<string, string>>& Movies) {
    string UserInput;
    while (getline(cin, UserInput)) {
        if (UserInput == "GET ALL MOVIES") {
            PrintMovies(Movies);
        }
        if (UserInput.find("GET SCHEDULE ") != string::npos) {
            string MovieName = GetMovieNameFromInput(UserInput);
            if (MovieExists(Movies, MovieName)) {
                PrintMovieSchedule(Movies, MovieName);
                HTMLMovieSchedule(Movies, MovieName);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    vector<map<string, string>> Movies = ReadCSVFile(argv[1]);
    HandleUserInput(Movies);

    return 0;
}