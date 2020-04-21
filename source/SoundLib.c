/**
 *******************************************************************************
 * @file     SoundLib.c
 * @author   HENIUS (Paweł Witak)
 * @version  1.1.2
 * @date     02-04-2013
 * @brief    Implementacja biblioteki audio
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2013 HENIUS</center></h2>
 */

/* Include section -----------------------------------------------------------*/

// --->System files

#include <string.h>
#include <ctype.h>
#include <avr/pgmspace.h>

// --->User files

#include "SoundLib.h"

/* Variable section ----------------------------------------------------------*/

/*! Wskaźnik do struktury inicjalizującej */
SoundController_t SoundController;			
ESoundRequest_t SoundRequest;		/*!< Rodzaj żądania audio */
int16_t BeepTimer;					/*!< Timer krótkiego dźwięku */
uint8_t *CurrentRing;				/*!< Wskaźnik do aktualnego dzwonka */
RTTTLHeader_t CurrentHeader;		/*!< Aktualnie odczytany nagłówek dzwonka */
uint16_t RingIndex;					/*!< Indeks znaku w treści dzwonka */
uint16_t RingTimer;					/*!< Timer dzwonka */
/*! Częstotliwości nut */
uint16_t NoteFrequencies[] = 
{         
	440,							// a
    247,							// b lub h
    262,							// c
    294,							// d
    330,							// e
    349,							// f
    392								// g
};

/* Function section ----------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/**
 * @brief    Inicjalizacja biblioteki
 * @param    *soundController : wskaźnik do struktury inicjalizującej audio
 * @retval   None
 */
void InitSoundLib(SoundController_t *soundController)
{	
	SoundController = *soundController;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Ustawianie zadanej częstotliowści audio (rozpoczęcie odgrywania)
 * @param    freq : częstotliwość (w Hz)
 * @retval   None
 */
void SetSoundFreq(uint16_t freq)
{
	if (SoundController.SetFreq)
	{
		SoundController.SetFreq(freq);
	}
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Zatrzymanie odgrywania dźwięku
 * @param    None
 * @retval   None
 */
void StopSoundPlay(void)
{
	if (SoundController.StopSound)
	{
		SoundController.StopSound();
	}
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Pobieranie nagłówka dzwonka
 * @param    *ring : treść dzwonka
 * @param    *idx : indeks treści dzwonka
 * @retval   Odczytany nagłówek
 */
 RTTTLHeader_t GetRingHeader(uint8_t *ring, uint16_t *idx)
{
    // Zwracany nagłówek
    RTTTLHeader_t header;
	// Aktualnie przetwarzany znak dzwonka
	uint8_t currentChar;
    // Długość treści dzwonka
    header.RingLength = 
	#ifdef SL_AVR
		strlen_P((char*)&ring[0]);
	#else
		strlen(ring);	
	#endif
    // Tempo
    uint8_t tempo;
	
	// Ignorowanie nazwy
    while (
	#ifdef SL_AVR
		pgm_read_byte(&ring[*idx]) != ':')
	#else
		ring[*idx] != ':')
	#endif
    {
        (*idx)++;
    }
    (*idx)++;                       // Ignorowanie ':'

    // Pobieranie domyślnego czasu trwania
    if (
	#ifdef SL_AVR
		pgm_read_byte(&ring[*idx]) == 'd')
	#else
		ring[*idx] == 'd')
	#endif
    {
        (*idx)++; ; (*idx)++;       // Ignorowanie "d="
        header.Duration = 0;
        while (isdigit(
		#ifdef SL_AVR
			currentChar = pgm_read_byte(&ring[*idx])))
		#else
			currentChar = ring[*idx]))
		#endif
        {
            header.Duration = (header.Duration * 10) + (currentChar - '0');
			(*idx)++;
        }

        (*idx)++;                   // Ignorowanie ','
    }

    // Pobieranie domyślnej oktawy
    if (
	#ifdef SL_AVR
		pgm_read_byte(&ring[*idx]) == 'o')
	#else
		ring[*idx] == 'o')
	#endif
    {
        (*idx)++; ; (*idx)++;       // Ignorowanie "o="
        header.Octave = 
		#ifdef SL_AVR
			pgm_read_byte(&ring[(*idx)++]) - '0';
		#else
			ring[(*idx)++] - '0';
		#endif

        (*idx)++;                   // Ignorowanie ','
    }

    // Pobieranie tempa BPM
    if (
	#ifdef SL_AVR
		pgm_read_byte(&ring[*idx]) == 'b')
	#else
		ring[*idx] == 'b')
	#endif
    {
        (*idx)++; ; (*idx)++;       // Ignorowanie "b="
        tempo = 0;
        while (isdigit(
		#ifdef SL_AVR
			currentChar = pgm_read_byte(&ring[*idx])))
		#else
			currentChar = ring[*idx]))
		#endif
        {
            tempo = (tempo * 10) + (currentChar - '0');
			(*idx)++;
        }

        // Obliczanie czasu całej nuty
        header.WholeNoteDuration = WHOLE_NOTE_DUR(tempo);

        (*idx)++;                   // Ignorowanie ','
    }

    return header;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Pobieranie najbliższej nuty
 * @param    *ring : treść dzwonka
 * @param    *idx : indeks treści dzwonka
 * @param    *header : aktualnie odczytany nagłówek dzwonka
 * @retval   Odczytana nuta
 */
Note_t GetNote(uint8_t* ring, uint16_t *idx, RTTTLHeader_t *header)
{
    // Zwracana nuta
    Note_t note;
    // Nuta (wysokość)
    uint8_t pitch;
    // Flaga oznaczająca istnienie znaku '#'
    bool isSharpExist = false;
    // Oktawa
    uint8_t octave = 0;
	// Aktualnie przetwarzany znak
	uint8_t currentChar;

    // Pobieranie wielkości nuty (jeśli dostępna)
    note.Duration = 0;
    while (isdigit(
	#ifdef SL_AVR
		currentChar = pgm_read_byte(&ring[*idx])))
	#else
		currentChar = ring[*idx]))
	#endif
    {
        note.Duration = note.Duration * 10 + currentChar - '0';
		(*idx)++;
    }

    // Ustalanie czasu trwania nuty
    if (note.Duration != 0)
    {
        note.Duration = header->WholeNoteDuration / note.Duration;
    }
    else
    {
        // Domyślna długość nuty
        note.Duration = header->WholeNoteDuration / header->Duration;
    }

    // Pobieranie nuty
    pitch = 
	#ifdef SL_AVR
		pgm_read_byte(&ring[(*idx)++]);
	#else
		ring[(*idx)++];
	#endif	

    // Czy jest dostępny znak '#'?
    if (
	#ifdef SL_AVR	
		pgm_read_byte(&ring[*idx]) == '#')
	#else
		ring[*idx] == '#')
	#endif
    {
        (*idx)++;
        isSharpExist = true;
    }

    // Czy jest dostępny znak '.'?
    if (
	#ifdef SL_AVR
		pgm_read_byte(&ring[*idx]) == '.')
	#else
		ring[*idx] == '.')
	#endif
    {
        (*idx)++;
        note.Duration = note.Duration * 1.5;
    }

    // Pobieranie oktawy nuty (jeśli dostępna)
    if (isdigit(
	#ifdef SL_AVR
		currentChar = pgm_read_byte(&ring[*idx])))
	#else
		currentChar = ring[*idx]))
	#endif
    {
        octave = currentChar - '0';
		(*idx)++;
    }
    else
    {        
        octave = header->Octave;	// Domyślna oktawa
    }

    // Obliczanie częstotliwości nuty
    if (pitch != 'p')
    {
        if (pitch != 0)
        {
            note.Frequency = NoteFrequencies[pitch - 'a'] * (octave - 3);

            // Ewentualne podwyższanie nuty o pół ton
            note.Frequency = note.Frequency * (isSharpExist ? 1.06 : 1);
        }
    }
    else
    {
        note.Frequency = 0;
    }

    // Ignorowanie znaku ','
    if (
	#ifdef SL_AVR
		pgm_read_byte(&ring[*idx]) == ',')
	#else
		ring[*idx] == ',')
	#endif
	
    {
        (*idx)++;
    }

    return note;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Funkcja obsługi biblioteki dźwiękowej
 * @param    None
 * @retval   None
 */
void SoundLibHandler(void)
{
	static Note_t note;				// Aktualnie grana nuta
	
	switch (SoundRequest)
	{
		// Odgrywanie krótkiego dźwięku
		case SR_BEEP:
			if (BeepTimer >= 0 && !(--BeepTimer))
			{	
				// Zatrzymanie odgrywania dźwięku			
				StopSoundPlay();
				SoundRequest = SR_NO_SOUND;
			}
			
			break;
			
		// Odtwarzanie dzwonka
		case SR_RING:
			note = GetNote(CurrentRing, &RingIndex, &CurrentHeader);
			RingTimer = note.Duration - 1;
			SoundRequest = SR_NOTE_WAIT;
			
			if (note.Frequency)
            {	
                SetSoundFreq(note.Frequency);
            }
			
		// Oczekiwanie na zakończenie grania nuty
		case SR_NOTE_WAIT:
			if (!(--RingTimer))
            {
				RingTimer = NOTE_DELAY;
				StopSoundPlay();
                SoundRequest = SR_NOTE_DELAY;
            }
		
			break;
			
		// Przerwa między nutami
		case SR_NOTE_DELAY:
			if (!(RingTimer--))
            {
				SoundRequest = RingIndex < CurrentHeader.RingLength ? 
					SR_RING : SR_NO_SOUND;
            }
		
			break;
			
		// Stan zatrzymania odgrywania dźwięk
		case SR_NO_SOUND: 
			StopSoundPlay();
			
		// Stan uśpienia modułu audio
		case SR_SUSPENDED:
			SoundRequest = SR_SUSPENDED;
			
			break;	
	}
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Generowanie dźwięku o zadanej częstotliwości i czasie trwania
 * @param    freq : częstotliwość (w Hz)
 * @param    duration : czas trwania (w ms), dla < 0 - granie nieskończone
 * @retval   None
 */
void Beep(uint16_t freq, int16_t duration)
{
	BeepTimer = duration;
	SetSoundFreq(freq);
	SoundRequest = SR_BEEP;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Odgrywanie dzwonka w formacie RTTTL
 * @param    *ring : wskaźnik do treści dzwonka
 * @retval   None
 */
void PlayRing(uint8_t *ring)
{
	CurrentRing = ring;				// Zapamiętanie dzwonka	
	RingIndex = 0;					// Kasowanie indeksu dzwonka
	
	// Pobieranie nagłówka
	CurrentHeader = GetRingHeader(ring, &RingIndex);
	
	SoundRequest = SR_RING;			// Rodzaj żądania		
}

/******************* (C) COPYRIGHT 2013 HENIUS *************** END OF FILE ****/
