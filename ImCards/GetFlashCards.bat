@echo off

cd Install/Release

if exist FlashCards (
	echo Update flashcards
	cd FlashCards
	git pull
) else (
	echo Download flashcards
	git clone https://github.com/P20H/FlashCards.git
)
