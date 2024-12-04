from distutils.core import setup
setup(
  name = 'QuickBPE',         # How you named your package folder (MyLib)
  packages = ['QuickBPE'],   # Chose the same as "name"
  version = '0.1',      # Start with a small number and increase it with every change you make
  license='MIT',        # Chose a license from here: https://help.github.com/articles/licensing-a-repository
  description = 'A fast bpe implementation in C',   # Give a short description about your library
  author = 'Johannes Voderholzer',                   # Type in your name
  author_email = 'invenis2@gmail.com',      # Type in your E-Mail
  url = 'https://github.com/JohannesVod/QuickBPE',   # Provide either the link to your github or to your website
  download_url = 'https://github.com/JohannesVod/QuickBPE/archive/refs/tags/bpe.tar.gz',    # I explain this later on
  keywords = ['BPE', 'LLM', 'tokenization'],   # Keywords that define your package best
  install_requires=[            # I get to this in a second
        'numpy',
        'tiktoken',
        'regex'
      ],
  classifiers=[
    'Development Status :: 3 - Alpha',      # Chose either "3 - Alpha", "4 - Beta" or "5 - Production/Stable" as the current state of your package
    'Intended Audience :: Developers',      # Define that your audience are developers
    'Topic :: AI developement :: Build Tools',
    'License :: OSI Approved :: MIT License',   # Again, pick a license
    'Programming Language :: Python :: 3',      #Specify which pyhton versions that you want to support
    'Programming Language :: Python :: 3.8',
    'Programming Language :: Python :: 3.11',
    'Programming Language :: Python :: 3.12',
  ],
)