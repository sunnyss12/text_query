有三个程序dict server client
dict：如果没有词频文件和倒排索引文件，首先需要运行dict程序生成词频文件和倒排索引文件
server:词频文件和倒排索引文件生成了之后就可以运行server启动服务器了。
注意：dict只运行一次就可以了。
client：服务器启动了之后，可以运行client，并从终端输入查询词，就可以从服务器获取推荐词了。
编译三个程序的方法：
make server
make dict
make client
注意：
    因为语料库和分词库太大没有提供，语料库应该放到./data/复旦文本分类测试语料目录中，这个目录应该存放英文和中文语料库。该目录路径配置在./config/config.ini中的distdir中。
    因为中文分词使用的中科院的分词ICTCLAS系统，所以需要把中科院的分词库应该放到./data/Data目录下。分词库可以在http://ictclas.nlpir.org/newsdownloads?DocId=389下载。下载了NLPIR/ICTCLAS2015分词系统包后，把包里的Data放到./data/Data即可。
    test目录下的文件并不是测试文件，而是测试一下某些cpp文件是否正确。这些文件没什么用。

以后进行的修改：
    缓冲用memcached 或者 redis代替。memcached不是存到磁盘中，断电后缓存消息，而redis可以缓存到文件中，redis比较好。
    log现在只是简单的写到了终端，可以用log4cpp代替。

