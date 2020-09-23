import shutil
import utils
import sys
import os
import random

from typing import Tuple

NULL_STRING = " "

pattern_dict = {
    "test_patterns" : {
        " < " : " == "
    },

    "custom_patterns" : {
        " < " : 
            [" <= ", " > ", " == "],
        "==" : "!=",
        "!=" : "==",
        " && " : " || ",
        " || " : " && ",

        "++" :
            [ "+=2", "-=2" ],

        " + " : " - ",
        " * " : " + ",
        " - " : " + ",

        "break;" : "{;}",

        " free(": "// free(",

        "[" :
            [ "[ -1 + ", "[ 1 + ", "[ 0 * " ],

        "," :
            [ ", -2 * ", ", 2 * " ],

        "0x00" :
            [ "0x01", "0x55" ],
        "0x01" :
            [ "0x00", "0x05" ],

        "return " : 
            [ "return 2 * ", "return -2 * " ]
    },

    "default_patterns" : {
        " < " : 
            [ " != ", " > ", " <= ", " >= ", " == " ],
        " > " : 
            [ " != ", " < ", " <= ", " >= ", " == " ],
        "<=" : 
            [ " != ", " < ", " > ", " >= ",  "==" ],
        ">=" : 
            [ " != ", " < ", " <= ", " > ",  "==" ],
        "==" : 
            [ " != ", " = ", " < ",  " > ", " <= ", " >= " ],
        "!=" : 
            [ " == ", " = ", " < ",  " > ", " <= ", " >= " ],
        # " = " : 
        #     [ " == ", " != ", " < ",  " > ", " <= ", " >= ", " = 0 * ", " = 0 ;//", " = NULL; //", " = ! " ],

        " + " : 
            [ " - ", " * ", " / ", " % " ],
        " - " : 
            [ " + ", " * ", " / ", " % " ],
        
        " * " : 
            [ " + ", " - ", " / ", " % " ],

        # " / " : 
        #     [ " % ", " * ", " + ", " - " ],
        # " % " : 
        #     [ " / ", " + ", " - ", " * " ],

        " + 1" :
            [ " - 1", "+ 0", "+ 2", "- 2" ],
        " - 1" :
            [ " + 1", "+ 0", "+ 2", "- 2" ],

        # " & " : 
        #     [ " | ", " ^ " ],
        # " | " : 
        #     [ " & ", " ^ " ],
        # " ^ " : 
        #     [ " & ", " | " ],

        # " &= " : 
        #     [ " |= ", " ^= " ],
        # " |= " : 
        #     [ " &= ", " ^= " ],
        # " ^= " : 
        #     [ " &= ", " |= " ],

        # " ~" : 
        #     [ " !", NULL_STRING ],
        # " !" : 
        #     [ " ~", NULL_STRING ],

        " && " : 
            [ " & ", " || "," && !" ],

        " || " :
            [ " | ", " && ", " || !" ],

        " >> " : " << ",
        " << " : " >> ",

        " << 1" :
            [ " << 0"," << -1", "<< 2" ],
        " >> 1" :
            [ " >> 0", " >> -1", ">> 2" ],

        "++" : "--",
        "--" : "++",

        "++;" : 
            [ "--;", "+=2;", "-=2;" ],
        "++)" : 
            [ "--)", "+=2)", "-=2)" ],
        "--;" : 
            [ "++;", "+=2;", "-=2;" ],
        "--)" : 
            [ "++)", "+=2)", "-=2)" ],

        " true "  :  " false ",
        " false " :  " true  ",

        "if (" :
            [ "if ( ! ", "if ( ~ ", "if ( 1 || ", "if ( 0 && " ],
        "while (" :
            [ "while ( ! ", "while ( ~ ", "while ( 0 && " , "// while (", " if (", "if (!"],
        
        "break;" : "{;}",
        "continue;" : "{;}",
        "goto " : "//goto ",

        "return " : 
            [ "return 0; //", "return 1; //", "return NULL; //", "return -1; //", "return 2* ", "return -1 * " ],


        # for embedded systems

        "0x00" :
            [ "0x01", "0x05", "0x0A", "0x0F", "0xAA", "0x55", "0xFF" ],
        "0x01 " :
            [ "0x00 ", "0x05 ", "0x0A ", "0x0F " ],
        "0x05 " :
            [ "0x00 ", "0x01 ", "0x0A ", "0x0F " ],
        "0x0A " :
            [ "0x00 ", "0x01 ", "0x05 ", "0x0F " ],
        "0x0F " :
            [ "0x00 ", "0x01 ", "0x05 ", "0x0A " ],


        "0x55 " :
            [ "0x00 ", "0xAA ", "0xFF " ],
        "0xAA " :
            [ "0x00 ", "0x55 ", "0xFF " ],
        "0xFF " :
            [ "0x00 ", "0x55 ", "0xAA " ],
        "[" :
            [ "[ -1 + ", "[ 1 + ", "[ 0 * " ],

        "(": " (! ",

        ");":
            [ "*0);", "*-1);", "*2);" ],
        "," :
            [ ", ! ", ", 0 * ", ", -1 * ", ", 2 *" ],
        " ? " :
            [ " && 0 ? ", " || 1 ? " ],
        " int " :
            [" short int ", " char " ],
        " signed " : " unsigned ",
        " unsigned " : " signed ",
        " long " : 
            [ " int ", " short int ", " char " ],
        " float ": " int ",
        " double ": " int ",


        " free(": "// free(",

        "case ": "// case ",
        "default ": "// default ",

        # null terminate a string
        "\"": "\"\\0",

        "else {": "{",
        "else": "// else",
    }
}


class Occurrence():
    def __init__(self, pattern, file, line, index):
        self.pattern = pattern
        self.file = file
        self.line = line
        self.index = index
    
    def __str__(self):
        return f'Pattern {self.pattern.pattern} found at File: {self.file}, Line: {self.line}, Index: {self.index}'

class Pattern():
    def __init__(self, pattern, transformation):
        self.pattern = pattern
        self.transformation = transformation
    
    def __str__(self):
        return f'{self.pattern} : {self.transformation}'

class Mutator():

    def __init__(self, src, mutation_patterns, rng=random.Random()):

        def _merge(intervals):
            """ Turns `intervals` [[0,2],[1,5],[7,8]] to [[0,5],[7,8]].
            """
            out = []
            for i in sorted(intervals, key=lambda i: i[0]):
                if out and i[0] <= out[-1][1]:
                    out[-1][1] = max(out[-1][1], i[1])
                else:
                    out += i,
            return out
        
        def _flatten(intervals):
            """ Turns `intervals` [[0,5], [8,10]] to [0, 1, 2, 3, 4, 5, 8, 9, 10].
            
            Note that it is inclusive of the second value.
            """
            out = []
            for i in intervals:
                out += list(range(i[0], i[1] + 1))
            return out

        utils.yellow_print("CWD: {}".format(os.getcwd()))
        self.olds = [] # stores original file paths
        self.modified = [] # stores path to the modified file
        self.lines_to_mutate = {} # maps file to the lines that file should mutate
        self.src = src

        for f in self.src:
            # create copies of the original
            old = '{}.old'.format(f)
            shutil.copyfile(f, old)
            self.olds.append(old)
            self.modified.append(f)
            # process the line intervals into a list of line numbers
            if len(self.src[f]) == 0:
                self.lines_to_mutate[old] = list(range(1, len(open(f).read().split('\n')) + 1))
            else:
                self.lines_to_mutate[old] = _flatten(_merge(self.src[f]))
        self.rng = rng
        self.mutation_patterns = self.flattenPatterns(mutation_patterns)
        self.pattern_generator = self.mutation_patterns.copy()
        self.file_index = None
        
        # for randomization
        # how do I make sure the same Occurrence-Pattern pair doesn't mutate again?
        self.mutants = set()

    def restore(self):
        """ Restores source files by replacing src with old

        Path should be root directory /freertos
        """
        for i in range(len(self.src)):
            shutil.copyfile(self.olds[i], self.modified[i])

    def cleanup(self):
        """ Cleans up by deleting all old files
        """
        self.restore()
        for i in range(len(self.src)):
            os.remove(self.olds[i])

    def mutate(self, occurrence: Occurrence) -> Tuple[str, str]:
        """ Mutates given `occurrence` using `occurrence.pattern`

        Returns the original line and the mutated line

        See mutator.py for information on what a Occurrence object is
        """
        mutation_pattern = occurrence.pattern
        source_code = open(occurrence.file).read().split('\n')
        sys.stderr.write("\n==> @ Line: " + str(occurrence.line) + "\n\n")
        sys.stderr.write("Original Line  : " + source_code[occurrence.line - 1].strip() + "\n")
        mutated_line = (source_code[occurrence.line - 1][0:occurrence.index] + 
            source_code[occurrence.line - 1][occurrence.index:]
                .replace(mutation_pattern.pattern, mutation_pattern.transformation, 1))
        sys.stderr.write("After Mutation : " + mutated_line.strip() + "\n")
        self.write_to_file(occurrence.file.rstrip('.old'), source_code, occurrence.line - 1, mutated_line)
        return source_code[occurrence.line - 1], mutated_line
    
    def write_to_file(self, path, source_code, mutated_line_number, mutated_line):
        """ Writes a file to `path` by replacing line at `mutated_line_number` in `source_code`
        with `mutated_line`.

        Helper to separate write to file process with mutate function.
        """
        output_file = open(path, "w")
        for i in range(0,len(source_code)) :
            if i == mutated_line_number : 
                output_file.write("/* XXX: original code was : " + source_code[i] + " */\n")
                output_file.write(mutated_line + "\n")
            else :
                output_file.write(source_code[i] + "\n")
        output_file.close()
        sys.stderr.write("\nOutput written to " + path + "\n\n")

    def findOccurrences(self, mutation_pattern: Pattern):
        """ Finds all occurrences of given `mutation_pattern` in current Mutator

        Returns a list of Occurrence objects (see mutator.Occurrence in mutator.py)
        
        If there are multiple source files, searches all of them. Occurrence object
        contains fields to access file, line, and index within line to find substring.
        """
        occurrences = []
        m = mutation_pattern.pattern
        for f in self.olds:
            source_code = open(f).read().split('\n')
            for line in self.lines_to_mutate[f]:
                number_of_substrings_found = source_code[line - 1].count(m)
                mutate_at_index = 0
                if number_of_substrings_found > 0 :
                    for _ in range(0, number_of_substrings_found):
                        if mutate_at_index == 0 :
                            mutate_at_index = source_code[line - 1].index(m)
                        else :
                            mutate_at_index = source_code[line - 1].index(m, mutate_at_index + 1)
                        occurrences.append(Occurrence(mutation_pattern, f, line, mutate_at_index))
        return occurrences                      

    def flattenPatterns(self, mutation_patterns):
        """ Flattens `mutation_patterns` into a list of mutation patterns

        `mutation_patterns` is a dictionary with mutant patterns

        Returns a list of Pattern objects (see mutator.Pattern in mutator.py).

        For ease of definition, the mutation_patterns is supplied as a dictionary,
        either through the default patterns, or through the configs.

        This function will process the dictionary into a list by creating Pattern
        objects for each key - transformation pair.
        """
        mutation_list = []
        for m in mutation_patterns:
            if type(mutation_patterns[m]) != str:
                for transformation in mutation_patterns[m]:
                    mutation_list.append(Pattern(m, transformation))
            else:
                mutation_list.append(Pattern(m, mutation_patterns[m]))
        return mutation_list
    
    def getPatterns(self):
        """ Returns a list of flattened Pattern objects
        """
        return self.mutation_patterns
    
    def nextRandomPattern(self):
        """ Gets a random pattern

        Ensures that the same pattern is not selected twice
        """
        if len(self.pattern_generator) == 0:
            self.pattern_generator = self.mutation_patterns.copy()
        index = self.rng.randint(0, len(self.pattern_generator) - 1)
        ret = self.pattern_generator[index]
        self.pattern_generator[index] = self.pattern_generator[len(self.pattern_generator) - 1]
        self.pattern_generator[len(self.pattern_generator) - 1] = ret
        self.pattern_generator.pop()
        return ret

    def generateRandomMutant(self) -> Tuple[Pattern, Occurrence, str, str]:
        """ Generates a mutant by choosing random pattern then a random line

        Not recommended to use everytime, it is a better idea to set up your own
        runs with the other API methods.

        This can be used as an example.
        """
        # Reset pattern_generator to all patterns
        self.pattern_generator = self.mutation_patterns.copy()

        # Go through every pattern to find an occurrence
        while len(self.pattern_generator) > 0:
            pattern = self.nextRandomPattern()
            occurrences = self.findOccurrences(pattern)
            if len(occurrences) != 0:
                occurrence = self.rng.choice(occurrences)
                original_line, mutated_line = self.mutate(occurrence)
                return pattern, occurrence, original_line, mutated_line
        sys.stderr.write("Could not create a mutant. Please make sure it is a C file.\n")
        sys.stderr.write("You may need to indent your C file.\n")
        return None





        


