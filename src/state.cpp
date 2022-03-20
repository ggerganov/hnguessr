#include "state.h"

#include "utils.h"

#include <fstream>

namespace {
std::vector<Token> tokenize(const std::string& src, std::string & out) {
    std::vector<Token> tokens;

    std::string token;
    for (char c : src) {
        if (c == ' ' || c == '\n' || c == '\t' || c == ',' || c == '.' || c == ';' || c == ':' || c == '?' || c == '!' || c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']' || c == '\'' || c == '"') {
            if (!token.empty()) {
                if (token.size() > 0) {
                    tokens.push_back({ false, token });
                    token.clear();
                    out += kTokenSymbol;
                } else {
                    out += token;
                    token.clear();
                }
            }
            out += c;
        } else {
            token += c;
        }
    }

    if (!token.empty()) {
        tokens.push_back({ false, token });
        out += kTokenSymbol;
    }

    return tokens;
}
}

void State::clear() {
    stories.clear();
}

bool State::reload(const char * fname) {
    clear();

    {
        std::ifstream fin(fname);
        if (!fin) {
            printf("Failed to open %s\n", fname);
            return false;
        }

        std::string line;
        std::getline(fin, line);
        timestamp_s = std::stoi(line);
        std::getline(fin, line);
        curDay = std::stoi(line);
        curIssue = curDay - kFirstDay;
        while (std::getline(fin, line)) {
            Story story;
            story.by = line;
            std::getline(fin, line);
            story.descendants = line;
            if (story.descendants == "null") {
                story.descendants = "0";
            }
            std::getline(fin, line);
            story.id = line;
            std::getline(fin, line);
            story.score = line;
            std::getline(fin, line);
            story.time = line;
            std::getline(fin, line);

            Utils::replaceAll(line, "&#x2F;", "/");
            Utils::replaceAll(line, "&#x27;", "'");
            Utils::replaceAll(line, "&gt;",   ">");
            Utils::replaceAll(line, "–",      "-");
            Utils::replaceAll(line, "“",      "\"");
            Utils::replaceAll(line, "”",      "\"");
            Utils::replaceAll(line, "‘",      "'");
            Utils::replaceAll(line, "’",      "'");
            Utils::replaceAll(line, "„",      "'");
            Utils::replaceAll(line, "&quot;", "\"");
            Utils::replaceAll(line, "&amp;",  "&");
            Utils::replaceAll(line, "—",      "-");

            story.title = line;
            std::getline(fin, line);
            story.url = line;
            stories.push_back(story);
        }

        for (auto & story : stories) {
            printf("%s\n", story.title.c_str());

            story.tokens = ::tokenize(story.title, story.out);
            printf("%s\n", story.out.c_str());
            for (const auto& token : story.tokens) {
                printf("[%s] ", token.text.c_str());
            }
            printf("\n");
            printf("\n");
        }
    }

    loadConfig();
    saveConfig();
    generateHTML();

    return true;
}

bool State::loadConfig() {
    std::ifstream fin(getConfigFilename());
    if (fin) {
        printf("Loading config\n");
        for (auto & story : stories) {
            for (auto & token : story.tokens) {
                int v = 0;
                fin >> v;
                if (v == 1) {
                    token.toGuess = true;
                } else {
                    token.toGuess = false;
                }
            }
        }
    } else {
        printf("No config file found\n");
        for (auto & story : stories) {
            story.randomize();
        }
    }

    return true;
}

bool State::saveConfig() {
    printf("Saving config to %s\n", getConfigFilename().c_str());

    std::ofstream fout(getConfigFilename());

    for (const auto & story : stories) {
        for (const auto & token : story.tokens) {
            if (token.toGuess) {
                fout << 1 << std::endl;
            } else {
                fout << 0 << std::endl;
            }
        }
    }

    return true;
}

bool State::generateIndex(const char * filename, int nStories) {
    printf("Generating index of %d stories in '%s'\n", nStories, filename);

    std::ofstream fout(filename);

    fout << R"(
<html lang="en" op="news">
    <head>
        <meta charset="UTF-8">
        <meta name="referrer" content="origin">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <link rel="stylesheet" type="text/css" href="/news.css">
        <link rel="shortcut icon" href="/favicon.ico">

        <!-- Disable cache -->
        <meta http-equiv="Cache-Control" content="no-cache, no-store, must-revalidate" />
        <meta http-equiv="Pragma" content="no-cache" />
        <meta http-equiv="Expires" content="0" />

        <!-- FB Meta Tags -->
        <meta property="og:url" content=")" << kURL << R"(">
        <meta property="og:type" content="website">
        <meta property="og:title" content=")" << kTitle << R"(">
        <meta property="og:description" content=")" << kDescription << R"(">
        <meta property="og:image" content=")" << kURL << R"(/android-chrome-512x512.png">

        <!-- Twitter Meta Tags -->
        <meta name="twitter:title" content=")" << kTitle << R"(">
        <meta name="twitter:card" content="summary">
        <meta name="twitter:description" content=")" << kDescription << R"(">
        <meta name="twitter:image" content=")" << kURL << R"(/android-chrome-512x512.png" />
        <meta property="twitter:domain" content=")" << kDomain << R"(">

        <link rel="apple-touch-icon" sizes="180x180" href="/apple-touch-icon.png">
        <link rel="icon" type="image/png" sizes="32x32" href="/favicon-32x32.png">
        <link rel="icon" type="image/png" sizes="16x16" href="/favicon-16x16.png">
        <link rel="manifest" href="/site.webmanifest">

        <script async src="https://www.googletagmanager.com/gtag/js?id=G-X0H3L4WYME"></script>
        <script>
            window.dataLayer = window.dataLayer || [];
            function gtag(){dataLayer.push(arguments);}
            gtag('js', new Date());

            gtag('config', 'G-X0H3L4WYME');
        </script>

        <title>)" << kTitle << R"(</title>
    </head>
    <body style="display: none">
        <center>
            <table id="hnmain" border="0" cellpadding="0" cellspacing="0" width="85%" bgcolor="#f6f6ef">
                <tr>
                    <td bgcolor="#ff6600">
                        <table border="0" cellpadding="0" cellspacing="0" width="100%" style="padding:2px">
                            <tr>
                                <td style="width:18px;padding-right:4px">
                                    <a href=")" << kURL << R"(" tabindex="-1">
                                        <img src="/logo.gif" width="18" height="18" style="border:1px white solid;">
                                    </a>
                                </td>
                                <td style="line-height:12pt; height:10px;">
                                    <span class="pagetop">
                                        <b class="hnname"><a href="/" tabindex="-1">)" << kTitle << R"(</a></b>
                                        <a href="/?n=5"  tabindex="-1">5</a> |
                                        <a href="/?n=10" tabindex="-1">10</a> |
                                        <a href="/?n=20" tabindex="-1">20</a> |
                                        <a href="/?n=30" tabindex="-1">30</a> |
                                        <a href="#" tabindex="-1" onClick="showHelp();">help</a>
                                    </span>
                                </td>
                                <td style="text-align:right;padding-right:4px;">
                                    <span class="pagetop">
                                        )" << curIssue << R"( |
                                        <span title=")" << Utils::timestampFormat(timestamp_s, "%Y-%m-%d %H:%M:%S") << " UTC" << R"(">
                                            )" << Utils::timestampFormat(timestamp_s, "%Y-%m-%d") << R"(
                                        </span>
                                    </span>
                                </td>
                            </tr>
                        </table>
                    </td>
                </tr>
                <tr id="pagespace" title="" style="height:10px"></tr>
                <tr>
                    <td>
                        <table id="container-main" border="0" cellpadding="0" cellspacing="0" class="itemlist">
)";

    int gtid = 0;
    std::vector<std::string> answers;
    std::vector<int> tokenToStoryMap;
    for (int i = 0; i < nStories; ++i) {
        const auto & story = stories[i];
        fout << R"(
                            <tr class='athing' id='container-story-)" << i << R"('>
                                                                        <td align="right" valign="top" class="title">
                                                                        <span class="rank">)" << (i + 1) << R"(.</span>
                                                                                                               </td>
                                                                                                               <td valign="top" class="votelinks">
                                                                                                               <center>
                                                                                                               &nbsp;&nbsp;
                                                                                                               </center>
                                                                                                               </td>
                                                                                                               <td class="title">
                                                                                                               <span class="titlelink"> )";

        int tid = 0;
        for (const auto ch : story.out) {
            if (ch == kTokenSymbol) {
                if (story.tokens[tid].toGuess) {
                    fout << R"(<span id="input-)" << gtid << R"(" class="input" role="textbox" maxlength="10" tabindex=")" << (gtid + 1) << R"(" contenteditable="true">)" << "" << R"(</span>)";
                    answers.push_back(story.tokens[tid].text);
                    tokenToStoryMap.push_back(i);
                    ++gtid;
                } else {
                    fout << story.tokens[tid].text;
                }
                ++tid;
            } else {
                fout << ch;
            }
        }

        std::string domain = story.url;
        Utils::replaceAll(domain, "https://", "");
        Utils::replaceAll(domain, "http://", "");
        Utils::replaceAll(domain, "www.", "");
        domain = domain.substr(0, domain.find("/"));

        const std::string strTime = Utils::timestampFormat(stoi(story.time), "%Y-%m-%d %H:%M:%S");

        fout << R"(</span>
)";

        if (story.url != "null") {
            fout << R"(
                                    <span class="sitebit comhead"> (<a id="url-)" << i << R"(" href=")" << story.url << R"(" tabindex="-1" class="inactivelink"><span class="sitestr">)" << domain << R"(</span></a>)</span>
        )";
        }
        fout << R"(
                                </td>
                            </tr>
                            <tr id='container-info-)" << i << R"('>
                                <td colspan="2"></td>
                                <td class="subtext">
                                    <span class="score" id="score-)" << story.id << R"(">)" << story.score << R"( points</span> by <span class="hnuser">)" << story.by << R"(</span>
                                    <span class="age" title=")" << strTime << R"( UTC">)" << strTime << R"(</span>
                                    <span id="unv_)" << story.id << R"("></span> |
                                    <a id="comments-)" << i << R"(" href="https://news.ycombinator.com/item?id=)" << story.id << R"(" class="inactivelink" tabindex="-1">)" << story.descendants << R"(&nbsp;comments</a>
                                </td>
                            </tr>
                            <tr id='container-spacer-)" << i << R"(' class="spacer" style="height:5px"></tr>
)";
    }

    fout << R"(
                        </table>
                    </td>
                </tr>
                <tr>
                    <td>
                        <img src="/s.gif" height="10" width="0">
                        <table width="100%" cellspacing="0" cellpadding="1">
                            <tr>
                                <td bgcolor="#ff6600">
                                </td>
                            </tr>
                        </table>
                        <br>
                        <center>
                            <span id="result"></span>
                            <span id="result-share" style="display:none"></span>
                            <input type="button" id="share" value="Share" onclick="copyToClipboard('result-share');"></input>
                        </center>
                        <br>
                        <center>
                            <span class="yclinks">
                                <a href="https://github.com/ggerganov/hnguessr">source code</a>
                                |
                                <a href="https://twitter.com/ggerganov">twitter</a>
                                |
                                <a href="https://ggerganov.com/">home page</a>
                            </span>
                        </center>
                        <br>
                    </td>
                </tr>
            </table>
        </center>

        <script>
            var nPlay = findGetParameter("n") || localStorage.getItem("n") || 10;

            var nStories = )" << nStories << R"(;
            var curDay = )" << curDay << R"(;
            var curIssue = )" << curIssue << R"(;
            var answers = [
)";

    for (const auto & answer : answers) {
        // escape single quotes
        std::string escaped = answer;
        Utils::replaceAll(escaped, "'", "\\'");
        fout << "'" << escaped << "', ";
    }

    fout << R"(
            ];

            var inputState = [
)";

    for (const auto & answer : answers) {
        fout << "false, ";
    }

    fout << R"(
            ];

            var tokenToStoryMap = [
)";

    for (const auto & tokenToStory : tokenToStoryMap) {
        fout << tokenToStory << ", ";
    }

    fout << R"(
            ];

            function findGetParameter(parameterName) {
                var tmp = [];
                var result = null;
                var items = location.search.substr(1).split("&");

                for (var index = 0; index < items.length; index++) {
                    tmp = items[index].split("=");
                    if (tmp[0] === parameterName) result = decodeURIComponent(tmp[1]);
                }

                return result;
            }

            function copyToClipboard(elementId) {
                var text = document.getElementById(elementId).innerHTML;

                if (window.clipboardData && window.clipboardData.setData) {
                    window.clipboardData.setData("Text", text);
                }
                else if (document.queryCommandSupported && document.queryCommandSupported("copy")) {
                    var textarea = document.createElement("textarea");
                    textarea.textContent = text;
                    textarea.style.position = "fixed";
                    document.body.appendChild(textarea);
                    textarea.select();
                    textarea.setSelectionRange(0, 99999);
                    try {
                        document.execCommand("copy");
                    }
                    catch (ex) {
                        console.warn("Copy to clipboard failed.", ex);
                    }
                    finally {
                        document.body.removeChild(textarea);
                    }
                } else {
                    console.error('Failed to copy data to the clipboard');
                }

                if (navigator.share) {
                    navigator.share({
                                    title: document.title,
                                    text: text
                                    //,url: window.location.href
                    })
                        .then(() => console.log('Successful share'))
                        .catch(error => console.log('Error sharing:', error));
                }
            }

            // compute similarity score between 2 strings
            function similarity(s1, s2) {
                var longer = s1;
                var shorter = s2;
                if (s1.length < s2.length) {
                    longer = s2;
                    shorter = s1;
                }
                var longerLength = longer.length;
                if (longerLength == 0) {
                    return 1.0;
                }
                return (longerLength - editDistance(longer, shorter)) / parseFloat(longerLength);
            }

            // compute the edit distance between the 2 strings
            // https://en.wikipedia.org/wiki/Damerau%E2%80%93Levenshtein_distance
            function editDistance(s1, s2) {
                s1 = s1.toLowerCase();
                s2 = s2.toLowerCase();

                var distances = new Array();
                for (var i = 0; i <= s1.length; i++) {
                    distances[i] = new Array();
                    distances[i][0] = i;
                }
                for (var j = 0; j <= s2.length; j++) {
                    distances[0][j] = j;
                }

                for (var i = 1; i <= s1.length; i++) {
                    for (var j = 1; j <= s2.length; j++) {
                        if (s1.charAt(i - 1) == s2.charAt(j - 1)) {
                            distances[i][j] = distances[i - 1][j - 1];
                        } else {
                            var substitutionCost = 1;
                            distances[i][j] = Math.min(distances[i - 1][j] + 1, // deletion
                                                       distances[i][j - 1] + 1, // insertion
                                                       distances[i - 1][j - 1] + substitutionCost); // substitution
                        }
                        if (i > 1 && j > 1 && s1.charAt(i - 1) == s2.charAt(j - 2) && s1.charAt(i - 2) == s2.charAt(j - 1)) {
                            distances[i][j] = Math.min(distances[i][j], distances[i - 2][j - 2] + substitutionCost); // transposition
                        }
                    }
                }
                return distances[s1.length][s2.length];
            }

            function refreshHyperlinks() {
                for (var i = 0; i < nStories; ++i) {
                    var isSolved = true;
                    for (var j = 0; j < inputState.length; ++j) {
                        if (tokenToStoryMap[j] == i) {
                            if (!inputState[j]) {
                                isSolved = false;
                                break;
                            }
                        }
                    }

                    var elURL = document.getElementById("url-" + i);
                    if (elURL != null) {
                        if (isSolved) {
                            document.getElementById("url-" + i).classList.remove("inactivelink");
                        } else {
                            document.getElementById("url-" + i).classList.add("inactivelink");
                        }
                    }

                    var elStory = document.getElementById("story-" + i);
                    if (elStory != null) {
                        if (isSolved) {
                            document.getElementById("story-" + i).classList.remove("inactivelink");
                        } else {
                            document.getElementById("story-" + i).classList.add("inactivelink");
                        }
                    }

                    var elComments = document.getElementById("comments-" + i);
                    if (elComments != null) {
                        if (isSolved) {
                            document.getElementById("comments-" + i).classList.remove("inactivelink");
                        } else {
                            document.getElementById("comments-" + i).classList.add("inactivelink");
                        }
                    }
                }
            }

            function check(id) {
                var elInput = document.getElementById('input-' + id);
                var input = elInput.innerText;
                var answer = answers[id];

                if (input.length == 0) {
                    elInput.style.backgroundColor = '#ffc466';
                    return;
                }

                // remove whitespace
                input = input.replace(/\s/g, '');

                // handle iOS smart punctuation
                input = input.replace(/[\u2018\u2019\u201C\u201D]/g, (c) => '\'\'""'.substr('\u2018\u2019\u201C\u201D'.indexOf(c), 1));

                if (input.toLowerCase() == answer.toLowerCase()) {
                    elInput.style.backgroundColor = '#00ff0055';
                    elInput.innerText = input;
                    inputState[id] = true;
                    refreshHyperlinks();
                    return true;
                }

                var score = similarity(input, answer);

                var red   = 255.0;
                var green = 0.0;
                var blue  = 0.0;
                var alpha = 0.2 + (0.5 - 0.5*score);

                elInput.style.backgroundColor = 'rgba(' + red + ', ' + green + ', ' + blue + ', ' + alpha + ')';
                inputState[id] = false;
                refreshHyperlinks();

                return false;
            }

            function checkAll() {
                for (var i = 0; i < answers.length; ++i) {
                    check(i);
                }

                updateResult();
            }

            function updateResult() {
                var score = 0;

                for (var i = 0; i < inputState.length; ++i) {
                    if (inputState[i]) {
                        score++;
                    }
                }

                document.getElementById('result').innerText = 'Score: ' + score + '/' + answers.length;
                document.getElementById('result-share').innerText = 'HNGuessr ' + curIssue + ' ' + score + '/' + answers.length;
            }

            function updateLocalStorage() {
                var elements = document.getElementsByClassName("input");
                var inputs = [];
                if (localStorage.getItem(curDay) != null) {
                    inputs = JSON.parse(localStorage.getItem(curDay));
                }
                if (elements.length > inputs.length) {
                    inputs = [];
                    for (var i = 0; i < elements.length; ++i) {
                        inputs.push(elements[i].innerText);
                    }
                } else {
                    for (var i = 0; i < elements.length; ++i) {
                        inputs[i] = elements[i].innerText;
                    }
                }
                localStorage.setItem(curDay, JSON.stringify(inputs));
            }

            function init() {
                localStorage.setItem("n", nPlay);

                // delete all elements with id "container-story-k" for k >= nPlay
                for (var i = nPlay; i < nStories; ++i) {
                    var elStory = document.getElementById("container-story-" + i);
                    if (elStory != null) {
                        elStory.parentNode.removeChild(elStory);
                    }
                    var elInfo = document.getElementById("container-info-" + i);
                    if (elInfo != null) {
                        elInfo.parentNode.removeChild(elInfo);
                    }
                    var elSpacer = document.getElementById("container-spacer-" + i);
                    if (elSpacer != null) {
                        elSpacer.parentNode.removeChild(elSpacer);
                    }
                }
                nStories = nPlay;

                // remove answers for which tokenToStory is >= nPlay
                for (var i = 0; i < tokenToStoryMap.length; ++i) {
                    if (tokenToStoryMap[i] >= nPlay) {
                        answers.splice(i, 1);
                        inputState.splice(i, 1);
                        tokenToStoryMap.splice(i, 1);
                        --i;
                    }
                }

                // enable display
                var elBody = document.getElementsByTagName("body")[0];
                elBody.style.display = "block";

                if (localStorage.getItem(curDay) != null) {
                    inputs = JSON.parse(localStorage.getItem(curDay));
                    for (var i = 0; i < inputs.length; ++i) {
                        var elInput = document.getElementById('input-' + i);
                        if (elInput != null) {
                            elInput.innerText = inputs[i];
                        }
                    }
                }
                updateLocalStorage();

                var inputs = document.getElementsByClassName("input");
                for (var i = 0; i < inputs.length; i++) {
                    inputs[i].addEventListener("keydown", function(event) {
                        if (event.keyCode === 13 || event.keyCode === 32) {
                            event.preventDefault();
                        }
                        if (event.keyCode === 13) {
                            updateLocalStorage();

                            var id = this.id.substring('input-'.length);
                            var idNext = (parseInt(id) + 1) % answers.length;

                            checkAll();
                            var isCorrect = inputState[id];

                            updateResult();

                            var next = id < inputs.length - 1 ? document.getElementById("input-" + idNext) : document.getElementById("share");
                            if (next && isCorrect) {
                                next.focus();
                            }
                        }
                    });
                }

                updateResult();

                setTimeout(function() {
                    checkAll();
                }, 1000);
            }

            function showHelp() {
                alert('Can you guess the missing words from yesterday\'s top HN stories?\n\n' +
                      '- Enter your answers in the boxes below\n' +
                      '- Press enter to verify your answers\n' +
                      '- Correct answers are highlighted in green\n\n' +
                      'Come back tomorrow for more!');
            }

            window.addEventListener('load', init);
        </script>
    </body>
</html>
)";

    return true;
}

bool State::generateHTML() {
    bool result = true;

    result &= generateIndex("index.html", 30);

    return result;
}
