import Live
import cpp
from _Framework.ControlSurface import ControlSurface

VK_LBUTTON = int("0x01", 16)
VK_RBUTTON = int("0x02", 16)
VK_CANCEL = int("0x03", 16)
VK_MBUTTON = int("0x04", 16)
VK_XBUTTON1 = int("0x05", 16)
VK_XBUTTON2 = int("0x06", 16)

VK_LCONTROL = int("0xA2", 16)

VK_BACK = int("0x08", 16)
VK_TAB = int("0x09", 16)
VK_CLEAR = int("0x0C", 16)
VK_RETURN = int("0x0D", 16)
VK_SHIFT = int("0x10", 16)
VK_CONTROL = int("0x11", 16)
VK_MENU = int("0x12", 16)
VK_PAUSE = int("0x13", 16)
VK_CAPITAL = int("0x14", 16)
VK_KANA = int("0x15", 16)
VK_HANGUL = int("0x15", 16)
VK_IME_ON = int("0x16", 16)
VK_JUNJA = int("0x17", 16)
VK_FINAL = int("0x18", 16)
VK_HANJA = int("0x19", 16)
VK_KANJI = int("0x19", 16)
VK_IME_OFF = int("0x1A", 16)
VK_ESCAPE = int("0x1B", 16)
VK_CONVERT = int("0x1C", 16)
VK_NONCONVERT = int("0x1D", 16)
VK_ACCEPT = int("0x1E", 16)
VK_MODECHANGE = int("0x1F", 16)
VK_SPACE = int("0x20", 16)
VK_PRIOR = int("0x21", 16)
VK_NEXT = int("0x22", 16)
VK_END = int("0x23", 16)
VK_HOME = int("0x24", 16)
VK_LEFT = int("0x25", 16)
VK_UP = int("0x26", 16)
VK_RIGHT = int("0x27", 16)
VK_DOWN = int("0x28", 16)
VK_SELECT = int("0x29", 16)
VK_PRINT = int("0x2A", 16)
VK_EXECUTE = int("0x2B", 16)
VK_SNAPSHOT = int("0x2C", 16)
VK_INSERT = int("0x2D", 16)
VK_DELETE = int("0x2E", 16)
VK_HELP = int("0x2F", 16)


class Utilities(ControlSurface):
    def __init__(self, c_instance):
        ControlSurface.__init__(self, c_instance)
        self.c_instance = c_instance

        self.tracks: Live.Base.Vector[Live.Track.Track] = None
        self.selected_track: Live.Track.Track = None
        self.is_playing: bool = False

        self.song().add_is_playing_listener(self._on_playing_change)
        # self.song().add_tracks_listener(self._on_tracks_changed)
        self.song().view.add_detail_clip_listener(self._on_detail_clip_changed)

        self.hotkeys()

    def _on_playing_change(self):
        self.schedule_message(1, self._playing_change_listeners)

    def _playing_change_listeners(self):
        self.is_playing = self.song().is_playing

    def _on_tracks_changed(self):
        self.schedule_message(1, self._tracks_change_listeners)

    def _tracks_change_listeners(self):
        self.tracks = self.song().tracks
        for track in self.tracks:
            try:
                arrangement_clips = track.arrangement_clips
            except RuntimeError:
                continue
            with Undo():
                for clip in arrangement_clips:
                    clip.name = track.name
                    clip.color = track.color

    def _on_detail_clip_changed(self):
        self.schedule_message(1, self._change_clip_listeners)

    def _change_clip_listeners(self):
        self.selected_track = self.song().view.selected_track
        try:
            arrangement_clips = self.selected_track.arrangement_clips
        except RuntimeError:
            return
        for clip in arrangement_clips:
            clip.name = self.selected_track.name
            clip.color = self.selected_track.color

    def get_detail_clip(self, song: Live.Song.Song) -> Live.Clip.Clip | None:
        ptr: int = cpp.get_detail_clip()
        detail_clip: Live.Clip.Clip = song.view.detail_clip
        self.selected_track: Live.Track.Track = song.view.selected_track
        if detail_clip is not None:
            return detail_clip
        try:
            arrangement_clips = self.selected_track.arrangement_clips
        except RuntimeError:
            return None
        for clip in arrangement_clips:
            if (clip._live_ptr == ptr):
                return clip

    def get_selected_clips(self, song: Live.Song.Song) -> set[Live.Clip.Clip]:
        result: set[Live.Clip.Clip] = set()
        tracks: Live.Track.Track = song.tracks
        arrangement_clips: Live.Base.Vector[Live.Clip.Clip]
        sel_start: float = min(cpp.time_selection_start_time(),
                               cpp.time_selection_end_time())
        sel_end: float = max(cpp.time_selection_start_time(),
                             cpp.time_selection_end_time())
        for track in tracks:
            track: Live.Track.Track
            if not cpp.is_selected_track(track._live_ptr):
                continue
            try:
                arrangement_clips = track.arrangement_clips
            except RuntimeError:
                continue
            for clip in arrangement_clips:
                clip_start: float = clip.start_time
                clip_end: float = clip.end_time
                if clip_start <= sel_end and clip_end >= sel_start:
                    result.add(clip)
        return result

    def pitch_one_semitone(self, clip: Live.Clip.Clip, choice: bool):
        if (clip.is_midi_clip):
            notes = clip.get_all_notes_extended()
            for note in notes:
                if (choice):
                    note.pitch += 1
                else:
                    note.pitch -= 1
            clip.apply_note_modifications(notes)
        elif (clip.is_audio_clip):
            if (not clip.warping):
                clip.warping = True
            if (choice):
                clip.pitch_coarse += 1
            else:
                clip.pitch_coarse -= 1

    def pitch_one_octave(self, clip: Live.Clip.Clip, choice: bool):
        if (clip.is_midi_clip):
            notes = clip.get_all_notes_extended()
            for note in notes:
                if (choice):
                    note.pitch += 12
                else:
                    note.pitch -= 12
            clip.apply_note_modifications(notes)
        elif (clip.is_audio_clip):
            if (not clip.warping):
                clip.warping = True
            if (choice):
                clip.pitch_coarse += 12
            else:
                clip.pitch_coarse -= 12

    def hotkeys(self):
        if (cpp.is_key_pressed(int("0x37", 16))):
            with Undo():
                clips: set[Live.Clip.Clip] = self.get_selected_clips(self.song())
                for clip in clips:
                    self.pitch_one_semitone(clip, False)

        if (cpp.is_key_pressed(int("0x38", 16))):
            with Undo():
                clips: set[Live.Clip.Clip] = self.get_selected_clips(
                    self.song())
                for clip in clips:
                    self.pitch_one_semitone(clip, True)


        detail_clip = self.get_detail_clip(self.song())
        if detail_clip:
            is_playing: bool = self.is_playing
            if detail_clip.is_midi_clip:
                selected_notes: Live.Clip.MidiNoteVector = detail_clip.get_selected_notes_extended()
                is_midi_preview: bool = cpp.is_midi_preview()
                if is_midi_preview:
                    if is_playing:
                        cpp.send_midi_preview()  # DEACTIVATE
                    else:
                        if len(selected_notes) > 1:
                            cpp.send_midi_preview()  # DEACTIVATE
                else:
                    if not is_playing:
                        if len(selected_notes) <= 1:
                            cpp.send_midi_preview()  # ACTIVATE

        self.schedule_message(1, self.hotkeys)


class Undo (object):
    def __enter__(self):
        Live.Application.get_application().get_document().begin_undo_step()

    def __exit__(self, type, value, tb):
        Live.Application.get_application().get_document().end_undo_step()
