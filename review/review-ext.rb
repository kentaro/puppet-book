# -*- coding: utf-8 -*-

module ReVIEW

  module LaTeXUtils
    def escape_latex(str)
      str.gsub(METACHARS_RE) {|s|
        MATACHARS[s] or raise "unknown trans char: #{s}"
      }.gsub(/ /,' ')
    end
  end

  class LATEXBuilder

    def emlist(lines, caption = nil)
      blank
      if caption
        puts macro('reviewemlistcaption', "#{compile_inline(caption)}")
      end
      puts '\begin{reviewemlist}'
      lines.each do |line|
        str =  detab(line)
        puts folding_line(str)
      end
      puts '\end{reviewemlist}'
      blank
    end

    def cmd(lines, caption = nil)
      blank
      if caption
        puts macro('reviewcmdcaption', "#{compile_inline(caption)}")
      end
      puts '\begin{reviewcmd}'
      lines.each do |line|
        str =  detab(line)
        puts folding_line(str)
      end
      puts '\end{reviewcmd}'
      blank
    end

    def folding_line(str, size=76)
      u_str = unescape_latex(str)
      folded = ""
      while u_str.size > 0
        if folded.size > 0
          folded += '\UTF{23CE}'+"\n"
        end
        folded += escape_latex(u_str.slice!(0,size))
      end
      folded
    end
  end

end
