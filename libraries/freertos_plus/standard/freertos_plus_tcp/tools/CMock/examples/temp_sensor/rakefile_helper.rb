require 'yaml'
require 'fileutils'
require '../../vendor/unity/auto/unity_test_summary'
require '../../vendor/unity/auto/generate_test_runner'
require '../../vendor/unity/auto/colour_reporter'

module RakefileHelpers
  $return_error_on_failures = false

  C_EXTENSION = '.c'.freeze

  def load_configuration(config_file)
    $cfg_file = config_file
    $cfg = YAML.load(File.read($cfg_file))
    $colour_output = false unless $cfg['colour']
  end

  def configure_clean
    CLEAN.include($cfg['compiler']['build_path'] + '*.*') unless $cfg['compiler']['build_path'].nil?
  end

  def configure_toolchain(config_file = DEFAULT_CONFIG_FILE)
    config_file += '.yml' unless config_file =~ /\.yml$/
    load_configuration(config_file)
    configure_clean
  end

  def unit_test_files
    path = $cfg['compiler']['unit_tests_path'] + 'Test*' + C_EXTENSION
    path.tr!('\\', '/')
    FileList.new(path)
  end

  def local_include_dirs
    include_dirs = $cfg['compiler']['includes']['items'].dup
    include_dirs.delete_if { |dir| dir.is_a?(Array) }
    include_dirs
  end

  def extract_headers(filename)
    includes = []
    lines = File.readlines(filename)
    lines.each do |line|
      m = line.match(/^\s*#include\s+\"\s*(.+\.[hH])\s*\"/)
      unless m.nil?
        includes << m[1]
      end
    end
    includes
  end

  def find_source_file(header, paths)
    paths.each do |dir|
      src_file = dir + header.ext(C_EXTENSION)
      if File.exist?(src_file)
        return src_file
      end
    end
    nil
  end

  def tackit(strings)
    case strings
    when Array
      "\"#{strings.join}\""
    when /^-/
      strings
    when /\s/
      "\"#{strings}\""
    else
      strings
    end
  end

  def squash(prefix, items)
    result = ''
    items.each { |item| result += " #{prefix}#{tackit(item)}" }
    result
  end

  def build_compiler_fields
    command = tackit($cfg['compiler']['path'])
    defines = if $cfg['compiler']['defines']['items'].nil?
                ''
              else
                squash($cfg['compiler']['defines']['prefix'], $cfg['compiler']['defines']['items'])
              end
    options = squash('', $cfg['compiler']['options'])
    includes = squash($cfg['compiler']['includes']['prefix'], $cfg['compiler']['includes']['items'])
    includes = includes.gsub(/\\ /, ' ').gsub(/\\\"/, '"').gsub(/\\$/, '') # Remove trailing slashes (for IAR)
    { :command => command, :defines => defines, :options => options, :includes => includes }
  end

  def compile(file, _defines = [])
    compiler = build_compiler_fields
    cmd_str  = "#{compiler[:command]}#{compiler[:defines]}#{compiler[:options]}#{compiler[:includes]} #{file} " \
               "#{$cfg['compiler']['object_files']['prefix']}#{$cfg['compiler']['object_files']['destination']}"
    obj_file = "#{File.basename(file, C_EXTENSION)}#{$cfg['compiler']['object_files']['extension']}"
    execute(cmd_str + obj_file)
    obj_file
  end

  def build_linker_fields
    command = tackit($cfg['linker']['path'])
    options = if $cfg['linker']['options'].nil?
                ''
              else
                squash('', $cfg['linker']['options'])
              end
    includes = if $cfg['linker']['includes'].nil? || $cfg['linker']['includes']['items'].nil?
                 ''
               else
                 squash($cfg['linker']['includes']['prefix'], $cfg['linker']['includes']['items'])
               end
    includes = includes.gsub(/\\ /, ' ').gsub(/\\\"/, '"').gsub(/\\$/, '') # Remove trailing slashes (for IAR)
    { :command => command, :options => options, :includes => includes }
  end

  def link_it(exe_name, obj_list)
    linker = build_linker_fields
    cmd_str = "#{linker[:command]}#{linker[:includes]} " +
              (obj_list.map { |obj| "#{$cfg['linker']['object_files']['path']}#{obj} " }).join +
              $cfg['linker']['bin_files']['prefix'] + ' ' +
              $cfg['linker']['bin_files']['destination'] +
              exe_name + $cfg['linker']['bin_files']['extension'] + " #{linker[:options]}"
    execute(cmd_str)
  end

  def build_simulator_fields
    return nil if $cfg['simulator'].nil?

    command = if $cfg['simulator']['path'].nil?
                ''
              else
                (tackit($cfg['simulator']['path']) + ' ')
              end
    pre_support = if $cfg['simulator']['pre_support'].nil?
                    ''
                  else
                    squash('', $cfg['simulator']['pre_support'])
                  end
    post_support = if $cfg['simulator']['post_support'].nil?
                     ''
                   else
                     squash('', $cfg['simulator']['post_support'])
                   end
    { :command => command, :pre_support => pre_support, :post_support => post_support }
  end

  def execute(command_string, verbose = true, ok_to_fail = false)
    report command_string
    output = `#{command_string}`.chomp
    report(output) if verbose && !output.nil? && !output.empty?
    unless (!$?.nil? && $?.exitstatus.zero?) || ok_to_fail
      raise "Command failed. (Returned #{$?.exitstatus})"
    end

    output
  end

  def report_summary
    summary = UnityTestSummary.new
    summary.root = HERE
    results_glob = "#{$cfg['compiler']['build_path']}*.test*"
    results_glob.tr!('\\', '/')
    results = Dir[results_glob]
    summary.targets = results
    report summary.run
    raise 'There were failures' if (summary.failures > 0) && $return_error_on_failures
  end

  def run_tests(test_files)
    report 'Running system tests...'

    # Tack on TEST define for compiling unit tests
    load_configuration($cfg_file)
    test_defines = ['TEST']
    $cfg['compiler']['defines']['items'] = [] if $cfg['compiler']['defines']['items'].nil?
    $cfg['compiler']['defines']['items'] << 'TEST'

    include_dirs = local_include_dirs

    # Build and execute each unit test
    test_files.each do |test|
      obj_list = []

      # Detect dependencies and build required required modules
      header_list = (extract_headers(test) + ['cmock.h'] + [$cfg[:cmock][:unity_helper_path]]).compact.uniq
      header_list.each do |header|
        # create mocks if needed
        next unless header =~ /Mock/

        require '../../lib/cmock.rb'
        @cmock ||= CMock.new($cfg_file)
        @cmock.setup_mocks([$cfg['compiler']['source_path'] + header.gsub('Mock', '')])
      end

      # compile all mocks
      header_list.each do |header|
        # compile source file header if it exists
        src_file = find_source_file(header, include_dirs)
        unless src_file.nil?
          obj_list << compile(src_file, test_defines)
        end
      end

      # Build the test runner (generate if configured to do so)
      test_base = File.basename(test, C_EXTENSION)
      runner_name = test_base + '_Runner.c'
      if $cfg['compiler']['runner_path'].nil?
        runner_path = $cfg['compiler']['build_path'] + runner_name
        test_gen = UnityTestRunnerGenerator.new($cfg_file)
        test_gen.run(test, runner_path)
      else
        runner_path = $cfg['compiler']['runner_path'] + runner_name
      end

      obj_list << compile(runner_path, test_defines)

      # Build the test module
      obj_list << compile(test, test_defines)

      # Link the test executable
      link_it(test_base, obj_list)

      # Execute unit test and generate results file
      simulator = build_simulator_fields
      executable = $cfg['linker']['bin_files']['destination'] + test_base + $cfg['linker']['bin_files']['extension']
      cmd_str = if simulator.nil?
                  executable
                else
                  "#{simulator[:command]} #{simulator[:pre_support]} #{executable} #{simulator[:post_support]}"
                end
      output = execute(cmd_str, true, true)
      test_results = $cfg['compiler']['build_path'] + test_base
      test_results += if output.match(/OK$/m).nil?
                        '.testfail'
                      else
                        '.testpass'
                      end
      File.open(test_results, 'w') { |f| f.print output }
    end
  end

  def build_application(main)
    report 'Building application...'

    obj_list = []
    load_configuration($cfg_file)
    main_path = $cfg['compiler']['source_path'] + main + C_EXTENSION

    # Detect dependencies and build required required modules
    include_dirs = local_include_dirs
    extract_headers(main_path).each do |header|
      src_file = find_source_file(header, include_dirs)
      unless src_file.nil?
        obj_list << compile(src_file)
      end
    end

    # Build the main source file
    main_base = File.basename(main_path, C_EXTENSION)
    obj_list << compile(main_path)

    # Create the executable
    link_it(main_base, obj_list)
  end
end
